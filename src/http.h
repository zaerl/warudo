#ifndef WRD_HTTP_H
#define WRD_HTTP_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

// Headers
wrd_code wrd_http_status(const char *status, warudo *config);
wrd_code wrd_http_content_type(const char *content_type, warudo *config);
wrd_code wrd_http_header(const char *status, const char *content_type, warudo *config);

// 200 OK
wrd_code wrd_http_ok(warudo *config);

// 201 Created
wrd_code wrd_http_created(unsigned long long int id, warudo *config);

// 201 Created (multiple)
wrd_code wrd_http_multi_created(unsigned long int count, warudo *config);

// 405 Method Not Allowed
wrd_code wrd_http_not_allowed(const char *allowed, warudo *config);

// 500 Internal Server Error
wrd_code wrd_http_server_error(const char *description, warudo *config);

// 400 Bad Request
wrd_code wrd_http_bad_request(const char *description, warudo *config);

// 404 Not Found
wrd_code wrd_http_not_found(warudo *config);

wrd_code wrd_http_printf(warudo *config, const char *format, ...);

// Write headers and content to the network
wrd_code wrd_http_flush(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WRD_HTTP_H
