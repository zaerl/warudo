#ifndef WRD_ENV_H
#define WRD_ENV_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get an integer from an environment variable.
WRD_API wrd_code wrd_get_env_int(int *result, const char *name);

// Get an string from an environment variable.
WRD_API wrd_code wrd_get_env_string(char **result, const char *name);

#ifdef __cplusplus
}
#endif

#endif // WRD_ENV_H
