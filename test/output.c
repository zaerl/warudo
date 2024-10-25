#include "../src/output.h"
#include "test.h"

void *test_output(void *arg) {
    ATT_ASSERT(wrd_escape_html(NULL), NULL, "NULL")
    ATT_ASSERT(wrd_escape_html(""), "", "Empty")
    ATT_ASSERT(wrd_escape_html("a-z"), "a-z", "Escape a-z")
    ATT_ASSERT(wrd_escape_html("<>&\""), "&lt;&gt;&amp;&quot;", "Special characters")

    return NULL;
}
