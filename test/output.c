#include "../src/output.h"
#include "test.h"

void *test_output(void *arg) {
    ATT_ASSERT(wrd_escape_html(NULL), NULL, "NULL")
    ATT_ASSERT(wrd_escape_html(""), "", "empty")
    ATT_ASSERT(wrd_escape_html("a-z"), "a-z", "a-z")
}
