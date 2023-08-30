#include "test.h"

void *test_route_app(void *arg) {
    ASSERT_CODE("NULL", WRD_ERROR, wrd_page_app, 0, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_page_app_keys, NULL)

    return NULL;
}
