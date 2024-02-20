#include <stdlib.h>
#include "../src/env.h"
#include "test.h"

void *test_env(void *arg) {
    ATT_ASSERT(wrd_get_env_int("__ZA_WARUDO", 0), 0, __FILE__)
    ATT_ASSERT(wrd_get_env_int("__ZA_WARUDO", 1), 1, __FILE__)
    ATT_ASSERT(wrd_get_env_string("PWD", "__ZA_WARUDO"), getenv("PWD"), "Default PWD")

    return NULL;
}
