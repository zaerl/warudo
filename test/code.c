#include "../src/code.h"
#include "test.h"

void *test_code(void *arg) {
    ATT_ASSERT((char*)wrd_error_description(WRD_OK, 0), "OK", "valid error code")
    ATT_ASSERT((char*)wrd_error_description(WRD_OK, 1), "Success", "valid error code ext")
    ATT_ASSERT((char*)wrd_error_description(WRD_UNKNOWN_ERROR - 1, 0),  NULL, "invalid error code")
    ATT_ASSERT((char*)wrd_error_description(WRD_UNKNOWN_ERROR - 1, 1), NULL, "invalid error code ext")
    ATT_ASSERT((char*)wrd_error_description(WRD_OK, 0), "OK", "valid error code")

    return NULL;
}
