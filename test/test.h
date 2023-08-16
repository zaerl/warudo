#ifndef WARUDO_TEST_H
#define WARUDO_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT(MESSAGE, EXPECTED, TEST_FUNC, ...) \
    _Generic((0, EXPECTED), \
        void*: warudo_assert_int((int)TEST_FUNC(__VA_ARGS__), (int)EXPECTED, #TEST_FUNC, MESSAGE), \
        int: warudo_assert_int(TEST_FUNC(__VA_ARGS__), EXPECTED, #TEST_FUNC, MESSAGE), \
        char*: warudo_assert_string(TEST_FUNC(__VA_ARGS__), EXPECTED, #TEST_FUNC, MESSAGE), \
        const char*: warudo_assert_string(TEST_FUNC(__VA_ARGS__), EXPECTED, #TEST_FUNC, MESSAGE), \
        default: warudo_assert_int(TEST_FUNC(__VA_ARGS__), EXPECTED, #TEST_FUNC, MESSAGE) \
    );

#define ASSERT_CODE(MESSAGE, EXPECTED, TEST_FUNC, ...) warudo_assert_code(TEST_FUNC(__VA_ARGS__), EXPECTED, #TEST_FUNC, MESSAGE);

#define MOCK_CONFIG warudo config;

void warudo_assert_code(int result, int expected, const char *func_name, const char *description);
void warudo_assert_int(int result, int expected, const char *func_name, const char *description);
void warudo_assert_string(char* result, char* expected, const char *func_name, const char *description);

#ifdef __cplusplus
}
#endif

#endif /* WARUDO_TEST_H */
