#ifndef ZAERL_HTML_H
#define ZAERL_HTML_H

char* zaerl_escape_html(const char* input);

int zaerl_content_type(const char* content_type);

int zaerl_environ(void);

#endif /* ZAERL_HTML_H */
