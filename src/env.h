#ifndef WRD_ENV_H
#define WRD_ENV_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

WRD_API wrd_code wrd_get_env_int(int *result, const char *name, int default_value);
WRD_API wrd_code wrd_get_env_string(char **result, const char *name, char *default_value);

#ifdef __cplusplus
}
#endif

#endif // WRD_ENV_H
