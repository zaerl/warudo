#ifndef WRD_TEST_H
#define WRD_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/warudo.h"
#include "attractor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOCK_CONFIG warudo config;
#define MOCK_CONFIG_S char *tmp_mock_file = NULL;

#define MOCK_FILE(NAME) \
    FILE *NAME = tmpfile(); \
    if(!NAME) ATT_ASSERT(0, 1, "tmpfile() failed");

#define FREE_MOCK_FILE_S \
    if(tmp_mock_file) { \
        remove(tmp_mock_file); \
        free((void*)tmp_mock_file); \
        tmp_mock_file = NULL; \
    }
#define MOCK_FILE_S(CONTENT) \
    FREE_MOCK_FILE_S \
    tmp_mock_file = wrd_mock_file(CONTENT); \
    if(!tmp_mock_file) ATT_ASSERT(0, 1, "wrd_mock_file() failed")

#define ASSERT_CODE(RESULT, EXPECTED, DESCRIPTION) wrd_assert_code(RESULT, EXPECTED, DESCRIPTION);

// Assert that the wrd_code is equal to the expected value.
void wrd_assert_code(int result, int expected, const char *description);

// Generate a temporary file with the given content.
char *wrd_mock_file(const char *content);

#ifdef __cplusplus
}
#endif

#endif /* WRD_TEST_H */
