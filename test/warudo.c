#include "test.h"

void *test_warudo(void *arg) {
    ASSERT_CODE(wrd_accept_connection(NULL), WRD_ERROR, "accept_connection")

    return NULL;
}
