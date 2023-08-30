#include "test.h"

void *test_init(void *arg) {
    ASSERT_CODE("NULL", WRD_ERROR, wrd_init, NULL, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_accept_connection, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_parse_query_string, NULL, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_accept_connection, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_after_connection, NULL)

#ifdef WRD_TIMING
    ASSERT_CODE("NULL", WRD_ERROR, wrd_start_time, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_end_time, NULL, 0, NULL)
#endif

    ASSERT_CODE("NULL", WRD_ERROR, wrd_close, NULL)

    return NULL;
}
