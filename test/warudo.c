#include "test.h"

void test_warudo(void) {
    INIT_TEST

    ASSERT_CODE("accept_connection", WARUDO_ERROR, warudo_accept_connection, NULL)
}
