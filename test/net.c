#include "../src/net.h"
#include "test.h"

void test_net(void) {
    INIT_TEST

    ASSERT_NULL("too short #1", warudo_get_formdata_boundary, "")
    ASSERT_NULL("too short #2", warudo_get_formdata_boundary, "\r")
    ASSERT_NULL("too short #3", warudo_get_formdata_boundary, "\r\n")
}
