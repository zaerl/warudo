/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include "test.h"

ATT_TEST(hpack) {
    ASSERT_CODE(WRD_OK, WRD_OK, "wrd_compress_header");
}

