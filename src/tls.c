/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/err.h>
#include <openssl/ssl.h>

#include "warudo.h"

// Log and clear the OpenSSL error queue.
static void wrd_tls_log_errors(warudo *config, const char *message) {
    unsigned long err = ERR_get_error();

    if(!err) {
        wrd_log_error(config, "%s\n", message);

        return;
    }

    while(err) {
        char buf[256];

        ERR_error_string_n(err, buf, sizeof(buf));
        wrd_log_error(config, "%s: %s\n", message, buf);
        err = ERR_get_error();
    }
}

// Init TLS: load certificate/key and configure SSL defaults. The SSL_CTX is held as an opaque
// pointer in config->tls_state.
WRD_API wrd_code wrd_init_tls(warudo *config) {
    CHECK_CONFIG

    if(!config->tls_enabled) {
        return WRD_OK;
    }

    if(!config->tls_cert_path || !config->tls_key_path) {
        wrd_log_error(config, "TLS enabled but cert/key paths not set%s\n", "");

        return WRD_ERROR;
    }

    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());

    if(!ctx) {
        wrd_tls_log_errors(config, "SSL_CTX_new failed");

        return WRD_MISSING_CRYPTO_ERROR;
    }

    // Allow only TLS 1.2 and later.
    if(!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION)) {
        wrd_tls_log_errors(config, "Failed to set minimum TLS version");
        goto error;
    }

    // Load server certificate chain.
    if(SSL_CTX_use_certificate_chain_file(ctx, config->tls_cert_path) != 1) {
        wrd_tls_log_errors(config, "Failed to load TLS certificate");
        goto error;
    }

    // Load private key.
    if(SSL_CTX_use_PrivateKey_file(ctx, config->tls_key_path, SSL_FILETYPE_PEM) != 1) {
        wrd_tls_log_errors(config, "Failed to load TLS private key");
        goto error;
    }

    if(SSL_CTX_check_private_key(ctx) != 1) {
        wrd_tls_log_errors(config, "TLS private key does not match certificate");
        goto error;
    }

    config->tls_state = ctx;
    wrd_log_info(config, "TLS initialized with certificate: %s\n", config->tls_cert_path);

    return WRD_OK;

error:
    SSL_CTX_free(ctx);

    return WRD_ERROR;
}

// Perform TLS handshake on the current client connection.
WRD_API wrd_code wrd_tls_handshake(warudo *config) {
    CHECK_CONFIG

    SSL_CTX *ctx = config->tls_state;

    if(!ctx) {
        return WRD_ERROR;
    }

    SSL *ssl = SSL_new(ctx);

    if(!ssl) {
        wrd_tls_log_errors(config, "SSL_new failed");

        return WRD_MEMORY_ERROR;
    }

    if(SSL_set_fd(ssl, config->client_fd) != 1) {
        wrd_tls_log_errors(config, "SSL_set_fd failed");
        SSL_free(ssl);

        return WRD_ERROR;
    }

    // Blocking socket: SSL_accept returns only on completion or failure.
    if(SSL_accept(ssl) != 1) {
        wrd_tls_log_errors(config, "TLS handshake failed");
        SSL_free(ssl);

        return WRD_ERROR;
    }

    config->tls_ssl = ssl;

    return WRD_OK;
}

// TLS read: read decrypted data from the TLS connection.
WRD_API int wrd_tls_read(void *ctx, unsigned char *buf, size_t len) {
    warudo *config = ctx;

    return SSL_read(config->tls_ssl, buf, len > INT_MAX ? INT_MAX : (int)len);
}

// TLS write: write data through the TLS connection.
WRD_API int wrd_tls_write(void *ctx, const unsigned char *buf, size_t len) {
    warudo *config = ctx;

    return SSL_write(config->tls_ssl, buf, len > INT_MAX ? INT_MAX : (int)len);
}

// Check if TLS has buffered decrypted data from a previous read.
WRD_API int wrd_tls_pending(warudo *config) {
    SSL *ssl = config->tls_ssl;

    if(!ssl) {
        return 0;
    }

    return SSL_pending(ssl) > 0;
}

// Close TLS session for the current connection.
WRD_API wrd_code wrd_tls_finish_request(warudo *config) {
    CHECK_CONFIG

    SSL *ssl = config->tls_ssl;

    if(!ssl) {
        return WRD_OK;
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    ERR_clear_error();
    config->tls_ssl = NULL;

    return WRD_OK;
}

// Free TLS state.
WRD_API wrd_code wrd_tls_close(warudo *config) {
    CHECK_CONFIG

    SSL_CTX *ctx = config->tls_state;

    if(!ctx) {
        return WRD_OK;
    }

    SSL_CTX_free(ctx);
    config->tls_state = NULL;

    return WRD_OK;
}
