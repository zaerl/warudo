#include "../src/version.h"
#include "test.h"

void *test_version(void *arg) {
    ATT_ASSERT(wrd_version(NULL), WRD_VERSION_NUMBER, "version")

    return NULL;
}
