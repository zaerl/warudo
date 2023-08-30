#include "../src/fcgi.h"
#include "test.h"

void *test_fcgi(void *arg) {
    // ASSERT("NULL", WRD_ERROR, wrd_fcgi_init)
    ASSERT("NULL", WRD_ERROR, wrd_fcgi_puts, NULL, NULL)
    ASSERT("NULL", NULL, wrd_fcgi_get_param, NULL, NULL)

    return NULL;
}
