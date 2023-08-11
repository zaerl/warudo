#ifndef WARUDO_TEST_H
#define WARUDO_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT_INTERNAL(MESSAGE, TEST, RES, ...) warudo_assert((TEST(__VA_ARGS__)) == RES, 0, #TEST, MESSAGE);
#define ASSERT_STRING_INTERNAL(MESSAGE, TEST, MUST_FREE, RES, ...) \
    buffer = TEST(__VA_ARGS__); \
    warudo_assert((strcmp(buffer, RES)) == 0, 0, #TEST, MESSAGE); \
    if (MUST_FREE) free(buffer);

#define ASSERT(MESSAGE, RES, TEST, ...) ASSERT_INTERNAL(MESSAGE, TEST, RES, __VA_ARGS__)
#define ASSERT_STRING(MESSAGE, RES, TEST, ...) ASSERT_STRING_INTERNAL(MESSAGE, TEST, 0, RES, __VA_ARGS__)
#define ASSERT_FSTRING(MESSAGE, RES, TEST, ...) ASSERT_STRING_INTERNAL(MESSAGE, TEST, 1, RES, __VA_ARGS__)

#define ASSERT_ERROR(MESSAGE, TEST, ...) ASSERT_INTERNAL(MESSAGE, TEST, WARUDO_ERROR, __VA_ARGS__)
#define ASSERT_OK(MESSAGE, TEST, ...) ASSERT_INTERNAL(MESSAGE, TEST, WARUDO_OK, __VA_ARGS__)
#define ASSERT_NULL(MESSAGE, TEST, ...) ASSERT_INTERNAL(MESSAGE, TEST, NULL, __VA_ARGS__)

#define DECLARE_TEST(NAME) void test_##NAME(void);
#define INIT_TEST printf("\x1b[34m%s\x1b[0m\n", __func__); char* buffer = NULL; (void*)buffer;
#define MOCK_CONFIG warudo config;

DECLARE_TEST(app)
DECLARE_TEST(db)
DECLARE_TEST(home)
DECLARE_TEST(init)
DECLARE_TEST(log)
DECLARE_TEST(net)
DECLARE_TEST(warudo)

typedef void (*warudo_test)(void);

void warudo_assert(int test, int wait, const char *func_name, const char *description);

#ifdef __cplusplus
}
#endif

#endif /* WARUDO_TEST_H */
