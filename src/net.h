#ifndef WARUDO_NET_H
#define WARUDO_NET_H

#include "warudo.h"

typedef int (*warudo_parse_formdata_callback)(const char*, long int, warudo*);

char* warudo_escape_html(const char* input);

char* warudo_url_decode(const char* input);

int warudo_is_valid_boundary(const char* boundary);

const char* warudo_get_formdata_boundary(const char* content_type);

warudo_code warudo_environ(warudo *config);

long warudo_content_length(warudo* config);

char* warudo_read_content(long int length, warudo* config);

warudo_code warudo_parse_formdata(const char* input, long int length, const char* boundary,
    warudo_parse_formdata_callback callback, warudo* config);

#endif /* WARUDO_NET_H */
