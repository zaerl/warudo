#ifndef WARUDO_HTTP_H
#define WARUDO_HTTP_H

#include "warudo.h"

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

#endif /* WARUDO_HTTP_H */
