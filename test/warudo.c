#include "test.h"

void test_warudo(void) {
    INIT_TEST

    ASSERT(WARUDO_OK == WARUDO_OK, "Warudo macro")
    ASSERT_ERROR(warudo_accept_connection(NULL), "warudo_accept_connection")
}
