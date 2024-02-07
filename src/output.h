#ifndef WRD_NET_H
#define WRD_NET_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*wrd_parse_formdata_callback)(const char*, long int, warudo*);

char *wrd_escape_html(const char *input);

char *wrd_url_decode(const char *input);

int wrd_is_valid_boundary(const char *boundary);

const char *wrd_get_formdata_boundary(const char *content_type);

wrd_code wrd_environ(warudo *config);

long wrd_content_length(warudo *config);

char *wrd_read_content(long int length, warudo *config);

wrd_code wrd_parse_formdata(const char *input, long int length, const char *boundary,
    wrd_parse_formdata_callback callback, warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WRD_NET_H
