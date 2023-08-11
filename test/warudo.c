#include "test.h"

void test_warudo(void) {
    INIT_TEST

    ASSERT_ERROR("accept_connection", warudo_accept_connection, NULL)
}
