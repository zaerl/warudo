/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include "test.h"

ATT_TEST(timing) {
    ASSERT_CODE(wrd_start_time(NULL), WRD_ERROR, "NULL")
    ASSERT_CODE(wrd_end_time(NULL, NULL), WRD_ERROR, "NULL")
}
