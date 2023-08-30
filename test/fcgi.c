#include "../src/fcgi.h"
#include "test.h"

void* test_fcgi(void* arg) {
    ASSERT("NULL", WRD_ERROR, wrd_fcgi_init, NULL)

    return NULL;
}
