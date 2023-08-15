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
        void*: warudo_assert_int((int)TEST_FUNC(__VA_ARGS__), (int)EXPECTED, 0, #TEST_FUNC, MESSAGE), \
        int: warudo_assert_int(TEST_FUNC(__VA_ARGS__), EXPECTED, 0, #TEST_FUNC, MESSAGE), \
        char*: warudo_assert_string(TEST_FUNC(__VA_ARGS__), EXPECTED, 0, #TEST_FUNC, MESSAGE), \
        const char*: warudo_assert_string(TEST_FUNC(__VA_ARGS__), EXPECTED, 0, #TEST_FUNC, MESSAGE), \
        default: warudo_assert_int(TEST_FUNC(__VA_ARGS__), EXPECTED, 0, #TEST_FUNC, MESSAGE) \
    );

#define ASSERT_ERROR(MESSAGE, TEST_FUNC, ...) ASSERT(MESSAGE, WARUDO_ERROR, TEST_FUNC, __VA_ARGS__)
#define ASSERT_OK(MESSAGE, TEST_FUNC, ...) ASSERT(MESSAGE, WARUDO_OK, TEST_FUNC, __VA_ARGS__)
#define ASSERT_NULL(MESSAGE, TEST_FUNC, ...) ASSERT(MESSAGE, NULL, TEST_FUNC, __VA_ARGS__)

#define DECLARE_TEST(NAME) void test_##NAME(void);
#define INIT_TEST printf("\x1b[34m%s\x1b[0m\n", __func__);

#define MOCK_CONFIG warudo config;

DECLARE_TEST(app)
DECLARE_TEST(db)
DECLARE_TEST(home)
DECLARE_TEST(init)
DECLARE_TEST(log)
DECLARE_TEST(net)
DECLARE_TEST(warudo)

typedef void (*warudo_test)(void);

void warudo_assert_int(int result, int expected, int wait, const char *func_name, const char *description);
void warudo_assert_string(char* result, char* expected, int wait, const char *func_name, const char *description);

#ifdef __cplusplus
}
#endif

#endif /* WARUDO_TEST_H */
