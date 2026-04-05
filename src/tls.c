/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>

#include "psa/crypto.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"

#include "warudo.h"

// TLS state held as opaque pointer in the warudo struct.
typedef struct wrd_tls_state {
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cert;
    mbedtls_pk_context pkey;
    int psa_initialized;
} wrd_tls_state;

// Per-connection TLS context stored in config->tls_ssl.
typedef struct wrd_tls_conn {
    mbedtls_ssl_context ssl;
    mbedtls_net_context net;
} wrd_tls_conn;

// Init TLS: load certificate/key and configure SSL defaults.
WRD_API wrd_code wrd_init_tls(warudo *config) {
    CHECK_CONFIG

    if(!config->tls_enabled) {
        return WRD_OK;
    }

    if(!config->tls_cert_path || !config->tls_key_path) {
        wrd_log_error(config, "TLS enabled but cert/key paths not set%s\n", "");

        return WRD_ERROR;
    }

    #if !defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_SRV_C) || \
        !defined(MBEDTLS_NET_C) || !defined(MBEDTLS_X509_CRT_PARSE_C)
    return WRD_MISSING_CRYPTO_ERROR;
    #endif

    wrd_tls_state *state = calloc(1, sizeof(wrd_tls_state));

    if(!state) {
        return WRD_MEMORY_ERROR;
    }

    int ret;

    mbedtls_ssl_config_init(&state->conf);
    mbedtls_x509_crt_init(&state->cert);
    mbedtls_pk_init(&state->pkey);

    // Initialize PSA Crypto (handles RNG internally in mbedtls 4.x).
    if(psa_crypto_init() != PSA_SUCCESS) {
        wrd_log_error(config, "Failed to initialize PSA Crypto%s\n", "");
        goto error;
    }

    state->psa_initialized = 1;

    // Configure SSL defaults for a TLS server.
    ret = mbedtls_ssl_config_defaults(&state->conf, MBEDTLS_SSL_IS_SERVER,
        MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);

    if(ret != 0) {
        wrd_log_error(config, "mbedtls_ssl_config_defaults failed: -0x%04X\n", -ret);
        goto error;
    }

    // Load server certificate chain.
    ret = mbedtls_x509_crt_parse_file(&state->cert, config->tls_cert_path);

    if(ret != 0) {
        wrd_log_error(config, "Failed to load TLS certificate: -0x%04X\n", -ret);
        goto error;
    }

    // Load private key.
    ret = mbedtls_pk_parse_keyfile(&state->pkey, config->tls_key_path, NULL);

    if(ret != 0) {
        wrd_log_error(config, "Failed to load TLS private key: -0x%04X\n", -ret);
        goto error;
    }

    // Bind certificate and key to the SSL config.
    ret = mbedtls_ssl_conf_own_cert(&state->conf, &state->cert, &state->pkey);

    if(ret != 0) {
        wrd_log_error(config, "mbedtls_ssl_conf_own_cert failed: -0x%04X\n", -ret);
        goto error;
    }

    // Enable debug output when log level is debug.
    /*if(config->log_level >= WRD_LOG_LEVEL_DEBUG) {
        mbedtls_ssl_conf_dbg(&state->conf, wrd_tls_debug, config);
        mbedtls_debug_set_threshold(4);
    }*/

    config->tls_state = state;
    wrd_log_info(config, "TLS initialized with certificate: %s\n", config->tls_cert_path);

    return WRD_OK;

error:
    mbedtls_pk_free(&state->pkey);
    mbedtls_x509_crt_free(&state->cert);
    mbedtls_ssl_config_free(&state->conf);

    if(state->psa_initialized) {
        mbedtls_psa_crypto_free();
    }

    free(state);

    return WRD_ERROR;
}

// Perform TLS handshake on the current client connection.
WRD_API wrd_code wrd_tls_handshake(warudo *config) {
    CHECK_CONFIG

    wrd_tls_state *state = config->tls_state;

    if(!state) {
        return WRD_ERROR;
    }

    wrd_tls_conn *conn = calloc(1, sizeof(wrd_tls_conn));

    if(!conn) {
        return WRD_MEMORY_ERROR;
    }

    mbedtls_ssl_init(&conn->ssl);
    conn->net.fd = config->client_fd;

    int ret = mbedtls_ssl_setup(&conn->ssl, &state->conf);

    if(ret != 0) {
        wrd_log_error(config, "mbedtls_ssl_setup failed: -0x%04X\n", -ret);
        mbedtls_ssl_free(&conn->ssl);
        free(conn);

        return WRD_ERROR;
    }

    mbedtls_ssl_set_bio(&conn->ssl, &conn->net,
        mbedtls_net_send, mbedtls_net_recv, NULL);

    // Perform the handshake.
    while((ret = mbedtls_ssl_handshake(&conn->ssl)) != 0) {
        if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            wrd_log_error(config, "TLS handshake failed: -0x%04X\n", -ret);
            mbedtls_ssl_free(&conn->ssl);
            free(conn);

            return WRD_ERROR;
        }
    }

    config->tls_ssl = conn;

    return WRD_OK;
}

// TLS read: read decrypted data from the TLS connection.
WRD_API int wrd_tls_read(void *ctx, unsigned char *buf, size_t len) {
    warudo *config = ctx;
    wrd_tls_conn *conn = config->tls_ssl;

    return mbedtls_ssl_read(&conn->ssl, buf, len);
}

// TLS write: write data through the TLS connection.
WRD_API int wrd_tls_write(void *ctx, const unsigned char *buf, size_t len) {
    warudo *config = ctx;
    wrd_tls_conn *conn = config->tls_ssl;

    return mbedtls_ssl_write(&conn->ssl, buf, len);
}

// Close TLS session for the current connection.
WRD_API wrd_code wrd_tls_finish_request(warudo *config) {
    CHECK_CONFIG

    wrd_tls_conn *conn = config->tls_ssl;

    if(!conn) {
        return WRD_OK;
    }

    mbedtls_ssl_close_notify(&conn->ssl);
    mbedtls_ssl_free(&conn->ssl);
    free(conn);
    config->tls_ssl = NULL;

    return WRD_OK;
}

// Free TLS state.
WRD_API wrd_code wrd_tls_close(warudo *config) {
    CHECK_CONFIG

    wrd_tls_state *state = config->tls_state;

    if(!state) {
        return WRD_OK;
    }

    mbedtls_pk_free(&state->pkey);
    mbedtls_x509_crt_free(&state->cert);
    mbedtls_ssl_config_free(&state->conf);

    if(state->psa_initialized) {
        mbedtls_psa_crypto_free();
    }

    free(state);
    config->tls_state = NULL;

    return WRD_OK;
}
