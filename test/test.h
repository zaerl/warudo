#ifndef WARUDO_TEST_H
#define WARUDO_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT(TEST, MESSAGE) warudo_assert((TEST), MESSAGE, 0);
#define ASSERT_ERROR(TEST, MESSAGE) warudo_assert((TEST) == WARUDO_ERROR, MESSAGE, 0);
#define ASSERT_OK(TEST, MESSAGE) warudo_assert((TEST) == WARUDO_OK, MESSAGE, 0);

#define DECLARE_TEST(NAME) void test_##NAME(void);
#define INIT_TEST printf("\x1b[34m%s\x1b[0m\n", __func__);

DECLARE_TEST(app)
DECLARE_TEST(db)
DECLARE_TEST(home)
DECLARE_TEST(init)
DECLARE_TEST(warudo)

typedef void (*warudo_test)(void);

void warudo_assert(int test, const char *message, int wait);

#ifdef __cplusplus
}
#endif

#endif /* WARUDO_TEST_H */
