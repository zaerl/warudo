#include "../src/fcgi.h"
#include "test.h"

void *test_fcgi(void *arg) {
    ASSERT_CODE(wrd_fcgi_init(), WRD_OK, "wrd_fcgi_init")
    ASSERT_CODE(wrd_fcgi_puts(NULL, NULL), WRD_ERROR, "wrd_fcgi_puts")
    ASSERT_CODE(wrd_fcgi_get_param(NULL, NULL), NULL, "wrd_fcgi_get_param")

    return NULL;
}
