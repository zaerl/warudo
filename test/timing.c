#include "../src/timing.h"
#include "test.h"

void *test_timing(void *arg) {
    ASSERT_CODE(wrd_start_time(NULL), WRD_ERROR, "NULL")
    ASSERT_CODE(wrd_end_time(NULL, NULL), WRD_ERROR, "NULL")

    return NULL;
}
