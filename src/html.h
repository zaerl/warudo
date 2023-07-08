#ifndef WARUDO_HTML_H
#define WARUDO_HTML_H

#include "warudo.h"

char* warudo_escape_html(const char* input);

int warudo_content_type(const char* content_type, warudo *config);

int warudo_not_allowed(const char* allowed, warudo* config);

int warudo_page_not_found(warudo* config);

int warudo_server_error(const char* description, warudo* config);

int warudo_bad_request(const char* description, warudo* config);

int warudo_environ(warudo *config);

#endif /* WARUDO_HTML_H */
