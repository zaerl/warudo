#include "test.h"

void *test_warudo(void *arg) {
    ASSERT_CODE("accept_connection", WRD_ERROR, wrd_accept_connection, NULL)

    return NULL;
}
