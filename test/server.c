#include "../src/server.h"
#include "test.h"

void *test_server(void *arg) {
    ASSERT_CODE(wrd_server_init(NULL), WRD_ERROR, "wrd_server_init NULL")
    ASSERT_CODE(wrd_accept_connection(NULL), WRD_ERROR, "wrd_accept_connection NULL")
    ASSERT_CODE(wrd_accept_connection(NULL), WRD_ERROR, "wrd_accept_connection NULL")
    ASSERT_CODE(wrd_after_connection(NULL), WRD_ERROR, "wrd_after_connection NULL")
    ASSERT_CODE(wrd_server_close(NULL), WRD_ERROR, "wrd_server_close NULL")

    return NULL;
}
