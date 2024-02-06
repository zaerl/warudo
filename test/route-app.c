#include "test.h"

void *test_route_app(void *arg) {
    ASSERT_CODE(wrd_page_app(0, NULL), WRD_ERROR, "NULL")
    ASSERT_CODE(wrd_page_app_keys(NULL), WRD_ERROR, "NULL")

    return NULL;
}
