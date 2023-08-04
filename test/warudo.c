#include "test.h"

void test_warudo(void) {
    ASSERT(WARUDO_OK == WARUDO_OK, "Warudo macro");

    // warudo_accept_connection
    ASSERT(warudo_accept_connection(NULL) == WARUDO_ERROR, "warudo_accept_connection");
}
