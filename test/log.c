#include "../src/log.h"
#include "test.h"

void test_log(void) {
    INIT_TEST

    ASSERT_ERROR("NULL", warudo_log, NULL, 0, "")
}
