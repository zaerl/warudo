#include "test.h"

void test_home(void) {
    INIT_TEST

    ASSERT_ERROR(warudo_page_home(NULL), "warudo_page_home NULL")
}
