#include "test.h"

void test_init(void) {
    INIT_TEST

    ASSERT_ERROR("NULL", warudo_init, NULL, NULL)
    ASSERT_ERROR("NULL", warudo_accept_connection, NULL)
    ASSERT_ERROR("NULL", warudo_parse_query_string, NULL, NULL)
    ASSERT_ERROR("NULL", warudo_accept_connection, NULL)
    ASSERT_ERROR("NULL", warudo_after_connection, NULL)

#ifdef WARUDO_TIMING
    ASSERT_ERROR("NULL", warudo_start_time, NULL)
    ASSERT_ERROR("NULL", warudo_end_time, NULL, 0, NULL)
#endif

    ASSERT_ERROR("NULL", warudo_close, NULL)
}
