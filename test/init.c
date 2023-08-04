#include "../src/warudo.h"
#include "test.h"

void test_init(void) {
    INIT_TEST

    ASSERT_ERROR(warudo_init(NULL, NULL), "warudo_init NULL")
    ASSERT_ERROR(warudo_accept_connection(NULL), "warudo_accept_connection NULL")
    ASSERT_ERROR(warudo_parse_query_string(NULL, NULL), "warudo_parse_query_string NULL")
    ASSERT_ERROR(warudo_accept_connection(NULL), "warudo_accept_connection NULL")
    ASSERT_ERROR(warudo_after_connection(NULL), "warudo_after_connection NULL")

#ifdef WARUDO_TIMING
    ASSERT_ERROR(warudo_start_time(NULL), "warudo_start_time NULL")
    ASSERT_ERROR(warudo_end_time(NULL, 0, NULL), "warudo_end_time NULL")
#endif

    ASSERT_ERROR(warudo_close(NULL), "warudo_close NULL")
}
