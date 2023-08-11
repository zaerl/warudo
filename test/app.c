#include "test.h"

void test_app(void) {
    INIT_TEST

    ASSERT_ERROR("NULL", page_app, 0, NULL)
    ASSERT_ERROR("NULL", page_app_keys, NULL)
}
