#ifndef WARUDO_TEST_H
#define WARUDO_TEST_H

#include "../src/warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT(TEST, MESSAGE) warudo_assert(TEST, MESSAGE, 0);
#define ASSERTO(TEST) warudo_assert(TEST, ##MESSAGE, 0);
#define DECLARE_TEST(NAME) void test_##NAME(void);
#define RUN_TEST(NAME) test_##NAME();

DECLARE_TEST(warudo)

typedef void (*warudo_test)(void);

void warudo_assert(int test, const char *message, int wait);

#ifdef __cplusplus
}
#endif

#endif /* WARUDO_TEST_H */
