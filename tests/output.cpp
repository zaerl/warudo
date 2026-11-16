/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include "test.h"

ATT_TEST(output) {
    ATT_ASSERT(WRD_OWNED(wrd_escape_html(NULL)), nullptr, "NULL");
    ATT_ASSERT(WRD_OWNED(wrd_escape_html("")), "", "Empty");
    ATT_ASSERT(WRD_OWNED(wrd_escape_html("a-z")), "a-z", "Escape a-z");
    ATT_ASSERT(WRD_OWNED(wrd_escape_html("<>&\"")), "&lt;&gt;&amp;&quot;", "Special characters");
}
