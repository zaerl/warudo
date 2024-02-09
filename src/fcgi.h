#ifndef WRD_FCGI_H
#define WRD_FCGI_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

wrd_code wrd_fcgi_accept(warudo *config);
wrd_code wrd_fcgi_finish_request(warudo *config);
wrd_code wrd_fcgi_free_request(warudo *config);
char *wrd_fcgi_get_param(const char *name, warudo *config);
wrd_code wrd_fcgi_getstr(char *str, int length, warudo *config);
wrd_code wrd_fcgi_puts(const char *str, warudo *config);
wrd_code wrd_fcgi_printf(warudo *config, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // WRD_FCGI_H
