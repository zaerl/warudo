#include <stdlib.h>
#include "../src/env.h"
#include "test.h"

void *test_env(void *arg) {
    int test_int;
    char *test_str;
    wrd_code code;

    code = wrd_get_env_int(&test_int, "__ZA_WARUDO", 0);
    ATT_ASSERT(code, WRD_DEFAULT, "Env 1")
    ATT_ASSERT(test_int, 0, "Env 2")

    code = wrd_get_env_int(&test_int, "__ZA_WARUDO", 1);
    ATT_ASSERT(code, WRD_DEFAULT, "Env 3")
    ATT_ASSERT(test_int, 1, "Env 4")

    code = wrd_get_env_string(&test_str, "PWD", "__ZA_WARUDO");
    ATT_ASSERT(code, WRD_OK, "Env 5")
    ATT_ASSERT(test_str, getenv("PWD"), "Default PWD")

    return NULL;
}
