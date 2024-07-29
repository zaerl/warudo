#include <string.h>

#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
// #include <test/certs.h>
#include <mbedtls/x509.h>
#include <mbedtls/ssl.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/timing.h>

#include "tls.h"

// Init TLS with default values.
WRD_API wrd_code wrd_init_tls(warudo *config) {
    #if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_ENTROPY_C) ||          \
        !defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_SRV_C) ||         \
        !defined(MBEDTLS_NET_C) || !defined(MBEDTLS_RSA_C) ||                 \
        !defined(MBEDTLS_CTR_DRBG_C) || !defined(MBEDTLS_X509_CRT_PARSE_C) || \
        !defined(MBEDTLS_TIMING_C) || !defined(MBEDTLS_FS_IO) ||              \
        !defined(MBEDTLS_PEM_PARSE_C)
    return WRD_MISSING_CRYPTO_ERROR;
    #endif

    wrd_code ret_code = WRD_ERROR;

    int ret = 1;
    // const char *pers = "ssl_fork_server";

    mbedtls_net_context listen_fd, client_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt srvcert;
    mbedtls_pk_context pkey;
    // int exit_code = MBEDTLS_EXIT_FAILURE;

    mbedtls_net_init(&listen_fd);
    mbedtls_net_init(&client_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_entropy_init(&entropy);
    mbedtls_pk_init(&pkey);
    mbedtls_x509_crt_init(&srvcert);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    /*psa_status_t status = psa_crypto_init();

    if (status != PSA_SUCCESS) {
        mbedtls_fprintf(stderr, "Failed to initialize PSA Crypto implementation: %d\n",
                        (int) status);
        goto exit;
    }*/

    signal(SIGCHLD, SIG_IGN);

    // Embedded.
    // mbedtls_x509_crt_parse
    // mbedtls_x509_crt_parse
    // mbedtls_pk_parse_key

    // Not Embedded.
    // mbedtls_x509_crt_parse_file
    // mbedtls_x509_crt_parse_file
    // mbedtls_pk_parse_keyfile

    if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER,
        MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        goto exit;
    }

    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    // mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

    // exit_code = MBEDTLS_EXIT_SUCCESS;
    ret_code = WRD_OK;

exit:
    mbedtls_net_free(&client_fd);
    mbedtls_net_free(&listen_fd);
    mbedtls_x509_crt_free(&srvcert);
    mbedtls_pk_free(&pkey);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    // mbedtls_psa_crypto_free();

    // mbedtls_exit(exit_code);

    return ret_code;
}
