#ifndef WRD_TEST_H
#define WRD_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT(MESSAGE, EXPECTED, TEST_FUNC, ...) \
    _Generic((0, EXPECTED), \
        void*: wrd_assert_int((int)TEST_FUNC(__VA_ARGS__), (int)EXPECTED, #TEST_FUNC, MESSAGE), \
        int: wrd_assert_int(TEST_FUNC(__VA_ARGS__), EXPECTED, #TEST_FUNC, MESSAGE), \
        char*: wrd_assert_string(TEST_FUNC(__VA_ARGS__), EXPECTED, #TEST_FUNC, MESSAGE), \
        const char*: wrd_assert_const_string(TEST_FUNC(__VA_ARGS__), EXPECTED, #TEST_FUNC, MESSAGE), \
        default: wrd_assert_int(TEST_FUNC(__VA_ARGS__), EXPECTED, #TEST_FUNC, MESSAGE) \
    );

#define ASSERT_CODE(MESSAGE, EXPECTED, TEST_FUNC, ...) wrd_assert_code(TEST_FUNC(__VA_ARGS__), EXPECTED, #TEST_FUNC, MESSAGE);

#define MOCK_CONFIG warudo config;
#define MOCK_FILE(NAME) FILE *NAME = tmpfile(); if(!NAME) wrd_assert_int(0, 1, "tmpfile", "tmpfile() failed");

void wrd_assert_code(int result, int expected, const char *func_name, const char *description);
void wrd_assert_int(int result, int expected, const char *func_name, const char *description);
void wrd_assert_string(char *result, char *expected, const char *func_name, const char *description);
void wrd_assert_const_string(const char *result, char *expected, const char *func_name, const char *description);

#ifdef __cplusplus
}
#endif

#endif /* WRD_TEST_H */
