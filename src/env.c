#include <stdlib.h>
#include "env.h"

int wrd_get_env_int(const char *name, int default_value) {
    char *env = getenv(name);

    if(env != NULL) {
        return atoi(env);
    }

    return default_value;
}

char* wrd_get_env_string(const char *name, char *default_value) {
    char *env = getenv(name);

    if(env != NULL) {
        return env;
    }

    return default_value;
}
