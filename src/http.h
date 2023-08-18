#ifndef WRD_HTTP_H
#define WRD_HTTP_H

#include "warudo.h"

wrd_code wrd_status(const char *status, warudo *config);

wrd_code wrd_content_type(const char *content_type, warudo *config);

wrd_code wrd_header(const char *status, const char *content_type, warudo *config);

wrd_code wrd_ok(warudo *config);

wrd_code wrd_created(unsigned long long int id, warudo *config);

wrd_code wrd_multi_created(unsigned long int count, warudo *config);

wrd_code wrd_not_allowed(const char *allowed, warudo *config);

wrd_code wrd_server_error(const char *description, warudo *config);

wrd_code wrd_bad_request(const char *description, warudo *config);

wrd_code wrd_not_found(warudo *config);

#endif // WRD_HTTP_H
