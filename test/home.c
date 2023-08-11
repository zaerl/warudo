#include "test.h"

void test_home(void) {
    INIT_TEST

    ASSERT_ERROR("NULL", warudo_page_home, NULL)
}
