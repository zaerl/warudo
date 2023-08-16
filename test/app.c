#include "test.h"

void* test_app(void* arg) {
    ASSERT_CODE("NULL", WARUDO_ERROR, page_app, 0, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, page_app_keys, NULL)

    return NULL;
}
