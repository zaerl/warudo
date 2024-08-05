#ifndef WRD_TEST_H
#define WRD_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/conf.h"
#include "../src/warudo.h"
#include "./attractor/attractor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOCK_CONFIG warudo config = {0}; wrd_config_init(&config, NULL);
#define MOCK_CONFIG_S char *tmp_mock_file = NULL;

#define WRD_TMP_FILE(NAME) \
    FILE *NAME = tmpfile(); \
    if(!NAME) ATT_ASSERT(0, 1, "tmpfile() failed");

#define WRD_MOCK_FILE(CONTENT) \
    FREE_MOCK_FILE \
    tmp_mock_file = wrd_mock_file(CONTENT); \
    if(!tmp_mock_file) ATT_ASSERT(0, 1, "wrd_mock_file() failed")

#define FREE_MOCK_FILE \
    if(tmp_mock_file) { \
        remove(tmp_mock_file); \
        free((void*)tmp_mock_file); \
        tmp_mock_file = NULL; \
    }

#define ASSERT_CODE(RESULT, EXPECTED, DESCRIPTION) wrd_assert_code(RESULT, EXPECTED, DESCRIPTION);

// Assert that the wrd_code is equal to the expected value.
void wrd_assert_code(int result, int expected, const char *description);

// Generate a temporary file with the given content.
char *wrd_mock_file(const char *content);

// Start tests declarations.
void *test_auth(void *arg);
void *test_code(void *arg);
void *test_conf(void *arg);
void *test_data(void *arg);
void *test_db(void *arg);
void *test_env(void *arg);
void *test_fs(void *arg);
void *test_hpack(void *arg);
void *test_http(void *arg);
void *test_log(void *arg);
void *test_net(void *arg);
void *test_output(void *arg);
void *test_query(void *arg);
void *test_route_app(void *arg);
void *test_route_home(void *arg);
void *test_server(void *arg);
void *test_timing(void *arg);
void *test_tls(void *arg);
void *test_version(void *arg);
void *test_worker(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* WRD_TEST_H */
