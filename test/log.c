#include "../src/warudo.h"
#include "../src/log.h"
#include "test.h"

void test_log(void) {
    INIT_TEST

    ASSERT_ERROR(warudo_log(NULL, 0, ""), "warudo_log NULL")
}
