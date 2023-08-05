#include "test.h"

void test_app(void) {
    INIT_TEST

    ASSERT_ERROR(page_app(0, NULL), "page_app NULL")
    ASSERT_ERROR(page_app_keys(NULL), "page_app_keys NULL")
}
