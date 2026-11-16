/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include "test.h"

ATT_TEST(output) {
    ATT_ASSERT(wrd_escape_html(NULL), NULL, "NULL");
    ATT_ASSERT(wrd_escape_html(""), "", "Empty");
    ATT_ASSERT(wrd_escape_html("a-z"), "a-z", "Escape a-z");
    ATT_ASSERT(wrd_escape_html("<>&\""), "&lt;&gt;&amp;&quot;", "Special characters");
}
