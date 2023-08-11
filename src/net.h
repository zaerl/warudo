#ifndef WARUDO_NET_H
#define WARUDO_NET_H

#include "warudo.h"

char* warudo_escape_html(const char* input);

void warudo_url_decode(const char* input);

char* warudo_get_formdata_boundary(char* content_type);

int warudo_status(const char* status, warudo* config);

int warudo_content_type(const char* content_type, warudo *config);

int warudo_header(const char* status, const char* content_type, warudo* config);

int warudo_ok(warudo* config);

int warudo_created(unsigned long long int id, warudo* config);

int warudo_not_allowed(const char* allowed, warudo* config);

int warudo_server_error(const char* description, warudo* config);

int warudo_bad_request(const char* description, warudo* config);

int warudo_page_not_found(warudo* config);

int warudo_environ(warudo *config);

long int warudo_content_length(warudo* config);

char* warudo_read_content(warudo* config, long int length);

#endif /* WARUDO_NET_H */
