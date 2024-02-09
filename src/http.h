#ifndef WRD_HTTP_H
#define WRD_HTTP_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

// Headers
wrd_code wrd_http_status(warudo *config, const char *status);
wrd_code wrd_http_content_type(warudo *config, const char *content_type);
wrd_code wrd_http_header(warudo *config, const char *status, const char *content_type);

// 200 OK
wrd_code wrd_http_ok(warudo *config);

// 201 Created
wrd_code wrd_http_created(warudo *config, unsigned long long int id);

// 201 Created (multiple)
wrd_code wrd_http_multi_created(warudo *config, unsigned long int count);

// 405 Method Not Allowed
wrd_code wrd_http_not_allowed(warudo *config, const char *allowed);

// 500 Internal Server Error
wrd_code wrd_http_server_error(warudo *config, const char *description);

// 400 Bad Request
wrd_code wrd_http_bad_request(warudo *config, const char *description);

// 404 Not Found
wrd_code wrd_http_not_found(warudo *config);

wrd_code wrd_http_puts(warudo *config, const char *str);
wrd_code wrd_http_printf(warudo *config, const char *format, ...);

// Write headers and content to the network
wrd_code wrd_http_flush(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WRD_HTTP_H
