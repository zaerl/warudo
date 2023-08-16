#include "test.h"

void test_warudo(void) {
    ASSERT_CODE("accept_connection", WARUDO_ERROR, warudo_accept_connection, NULL)
}
