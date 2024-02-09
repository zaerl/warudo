#include "../src/time.h"
#include "test.h"

void *test_time(void *arg) {
#ifdef WRD_TIMING
    ASSERT_CODE(wrd_start_time(NULL), WRD_ERROR, "NULL")
    ASSERT_CODE(wrd_end_time(NULL, 0, NULL), WRD_ERROR, "NULL")
#endif

    return NULL;
}
