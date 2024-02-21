#ifndef WRD_ENV_H
#define WRD_ENV_H

#ifdef __cplusplus
extern "C" {
#endif

int wrd_get_env_int(const char *name, int default_value);
char* wrd_get_env_string(const char *name, char *default_value);

#ifdef __cplusplus
}
#endif

#endif // WRD_ENV_H
