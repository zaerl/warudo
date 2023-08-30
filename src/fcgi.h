#ifndef WRD_FCGI_H
#define WRD_FCGI_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

wrd_code wrd_fcgi_init(void);
wrd_code wrd_fcgi_open_socket(const char *path, int backlog);
wrd_code wrd_fcgi_init_request(warudo *config);
wrd_code wrd_fcgi_puts(const char *str, warudo *config);
char *wrd_fcgi_get_param(const char *name, warudo *config);
wrd_code wrd_fcgi_printf(warudo *config, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // WRD_FCGI_H
