/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include "test.h"

ATT_TEST(version) {
    ATT_ASSERT(wrd_version(NULL), WRD_VERSION_NUMBER, "version");
}
