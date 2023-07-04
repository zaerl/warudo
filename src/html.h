#ifndef ZAERL_HTML_H
#define ZAERL_HTML_H

#include "zaerl.h"

char* zaerl_escape_html(const char* input);

int zaerl_content_type(const char* content_type, zaerl *config);

int zaerl_page_not_found(zaerl* config);

int zaerl_environ(zaerl *config);

#endif /* ZAERL_HTML_H */
