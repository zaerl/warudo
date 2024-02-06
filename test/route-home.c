#include "test.h"

void *test_route_home(void *arg) {
    ASSERT_CODE(wrd_page_home(NULL), WRD_ERROR, "NULL")

    return NULL;
}
