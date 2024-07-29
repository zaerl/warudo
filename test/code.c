#include "../src/code.h"
#include "test.h"

void *test_code(void *arg) {
    ATT_ASSERT((char*)wrd_error_description(WRD_OK, 0), "OK", "Valid error code")
    ATT_ASSERT((char*)wrd_error_description(WRD_OK, 1), "Success", "Valid error code ext")
    ATT_ASSERT((char*)wrd_error_description(WRD_UNKNOWN_ERROR - 1, 0),  NULL, "Invalid error code")
    ATT_ASSERT((char*)wrd_error_description(WRD_UNKNOWN_ERROR - 1, 1), NULL, "Invalid error code ext")

    return NULL;
}
