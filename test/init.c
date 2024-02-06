#include "test.h"

void *test_init(void *arg) {
    ASSERT_CODE(wrd_init(NULL, NULL), WRD_ERROR, "wrd_init NULL")
    ASSERT_CODE(wrd_accept_connection(NULL), WRD_ERROR, "wrd_accept_connection NULL")
    ASSERT_CODE(wrd_parse_query_string(NULL, NULL), WRD_ERROR, "wrd_parse_query_string NULL")
    ASSERT_CODE(wrd_accept_connection(NULL), WRD_ERROR, "wrd_accept_connection NULL")
    ASSERT_CODE(wrd_after_connection(NULL), WRD_ERROR, "wrd_after_connection NULL")

#ifdef WRD_TIMING
    ASSERT_CODE(wrd_start_time(NULL), WRD_ERROR, "NULL")
    ASSERT_CODE(wrd_end_time(NULL, 0, NULL), WRD_ERROR, "NULL")
#endif

    ASSERT_CODE(wrd_close(NULL), WRD_ERROR, "wrd_close NULL")

    return NULL;
}
