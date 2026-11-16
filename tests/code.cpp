/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include "test.h"

ATT_TEST(code) {
    ATT_ASSERT((char*)wrd_error_description(WRD_OK, 0), "OK", "Valid error code");
    ATT_ASSERT((char*)wrd_error_description(WRD_OK, 1), "Success", "Valid error code ext");
    ATT_ASSERT((char*)wrd_error_description(static_cast<wrd_code>(WRD_UNKNOWN_ERROR - 1), 0),  NULL, "Invalid error code");
    ATT_ASSERT((char*)wrd_error_description(static_cast<wrd_code>(WRD_UNKNOWN_ERROR - 1), 1), NULL, "Invalid error code ext");
}
