#include "../src/routes.h"
#include "test.h"

void *test_route_home(void *arg) {
    ASSERT_CODE(wrd_route_home(NULL), WRD_ERROR, "Home NULL")

    return NULL;
}
