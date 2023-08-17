#ifndef WARUDO_NET_H
#define WARUDO_NET_H

#include "warudo.h"

char* warudo_escape_html(const char* input);

char* warudo_url_decode(const char* input);

int is_valid_boundary(const char* boundary);

const char* warudo_get_formdata_boundary(const char* content_type);

warudo_code warudo_status(const char* status, warudo* config);

warudo_code warudo_content_type(const char* content_type, warudo *config);

warudo_code warudo_header(const char* status, const char* content_type, warudo* config);

warudo_code warudo_ok(warudo* config);

warudo_code warudo_created(unsigned long long int id, warudo* config);

warudo_code warudo_multi_created(unsigned long int count, warudo* config);

warudo_code warudo_not_allowed(const char* allowed, warudo* config);

warudo_code warudo_server_error(const char* description, warudo* config);

warudo_code warudo_bad_request(const char* description, warudo* config);

warudo_code warudo_page_not_found(warudo* config);

warudo_code warudo_environ(warudo *config);

long warudo_content_length(warudo* config);

char* warudo_read_content(long int length, warudo* config);

warudo_code warudo_parse_formdata(const char* input, long int length, const char* boundary,
    int (*callback)(const char*, long int, warudo*), warudo* config);

#endif /* WARUDO_NET_H */
