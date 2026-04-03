/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <signal.h>
#include <string.h>

#include "psa/crypto.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/debug.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"

#include "warudo.h"

// Init TLS with default values.
WRD_API wrd_code wrd_init_tls(warudo *config) {
    #if !defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_SRV_C) || \
        !defined(MBEDTLS_NET_C) || !defined(MBEDTLS_X509_CRT_PARSE_C) || \
        !defined(MBEDTLS_TIMING_C)
    return WRD_MISSING_CRYPTO_ERROR;
    #endif

    wrd_code ret_code = WRD_ERROR;
    int ret = 1;

    mbedtls_net_context server_fd;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    mbedtls_pk_context pkey;

    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_pk_init(&pkey);

    signal(SIGCHLD, SIG_IGN);

    // Initialize PSA Crypto (handles RNG internally in mbedtls 4.x).
    if (psa_crypto_init() != PSA_SUCCESS) {
        goto exit;
    }

    if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER,
        MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        goto exit;
    }

    // mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

    ret_code = WRD_OK;

exit:
    mbedtls_net_free(&server_fd);
    mbedtls_x509_crt_free(&cacert);
    mbedtls_pk_free(&pkey);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_psa_crypto_free();

    return ret_code;
}
