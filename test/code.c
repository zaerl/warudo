#include "../src/code.h"
#include "test.h"

void* test_code(void* arg) {
    ASSERT("valid error code", "OK", wrd_error_description, WRD_OK, 0)
    ASSERT("valid error code ext", "Success", wrd_error_description, WRD_OK, 1)
    ASSERT("invalid error code", NULL, wrd_error_description, WRD_UNKNOWN_ERROR - 1, 0)
    ASSERT("invalid error code ext", NULL, wrd_error_description, WRD_UNKNOWN_ERROR - 1, 1)

    return NULL;
}
