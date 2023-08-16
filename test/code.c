#include "../src/code.h"
#include "test.h"

void* test_code(void* arg) {
    ASSERT("valid error code", "OK", warudo_error_description, WARUDO_OK, 0)
    ASSERT("valid error code ext", "Success", warudo_error_description, WARUDO_OK, 1)
    ASSERT("invalid error code", NULL, warudo_error_description, WARUDO_UNKNOWN_ERROR - 1, 0)
    ASSERT("invalid error code ext", NULL, warudo_error_description, WARUDO_UNKNOWN_ERROR - 1, 1)

    return NULL;
}
