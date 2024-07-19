#include <stdlib.h>
#include <string.h>

#include "env.h"

WRD_API wrd_code wrd_get_env_int(int *result, const char *name, int default_value) {
    char *env = getenv(name);

    if(env != NULL) {
        *result = atoi(env);
    } else {
        *result = default_value;

        return WRD_DEFAULT;
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_get_env_string(char **result, const char *name, char *default_value) {
    char *env = getenv(name);

    if(env != NULL) {
        *result = strdup(env);
    } else {
        *result = strdup(default_value);

        return WRD_DEFAULT;
    }

    return WRD_OK;
}
