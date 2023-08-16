#include "test.h"

void test_init(void) {
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_init, NULL, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_accept_connection, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_parse_query_string, NULL, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_accept_connection, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_after_connection, NULL)

#ifdef WARUDO_TIMING
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_start_time, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_end_time, NULL, 0, NULL)
#endif

    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_close, NULL)
}
