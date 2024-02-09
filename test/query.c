#include "../src/query.h"
#include "test.h"

void *test_query(void *arg) {
    ASSERT_CODE(wrd_parse_query_string(NULL, NULL), WRD_ERROR, "wrd_parse_query_string NULL")

    return NULL;
}
