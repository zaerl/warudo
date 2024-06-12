#include "../src/hpack.h"
#include "test.h"

void *test_hpack(void *arg) {
    ASSERT_CODE(WRD_OK, WRD_OK, "wrd_compress_header")

    return NULL;
}

