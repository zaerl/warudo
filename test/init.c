#include "test.h"

void test_init(void) {
    ASSERT_ERROR(warudo_accept_connection(NULL), "warudo_accept_connection NULL")
}
