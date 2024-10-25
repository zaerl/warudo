#include <stdlib.h>
#include <string.h>

#include "env.h"

// Get an integer from an environment variable.
WRD_API wrd_code wrd_get_env_int(int *result, const char *name) {
    char *env = getenv(name);

    if(env != NULL) {
        *result = atoi(env);

        return WRD_OK;
    }

    return WRD_DEFAULT;
}

// Get a string from an environment variable.
WRD_API wrd_code wrd_get_env_string(char **result, const char *name) {
    char *env = getenv(name);

    if(env != NULL) {
        *result = strdup(env);

        return WRD_OK;
    }

    return WRD_DEFAULT;
}
