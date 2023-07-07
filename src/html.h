#ifndef ZAERL_HTML_H
#define ZAERL_HTML_H

#include "zaerl.h"

char* zaerl_escape_html(const char* input);

int zaerl_content_type(const char* content_type, zaerl *config);

int zaerl_not_allowed(const char* allowed, zaerl* config);

int zaerl_page_not_found(zaerl* config);

int zaerl_server_error(const char* description, zaerl* config);

int zaerl_bad_request(const char* description, zaerl* config);

int zaerl_environ(zaerl *config);

#endif /* ZAERL_HTML_H */
