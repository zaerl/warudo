#ifndef SERVER_H
#define SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "warudo.h"

// HTTP request methods
#define WRD_REQUEST_UNKNOWN 0
#define WRD_REQUEST_GET 1
#define WRD_REQUEST_POST 2

// Pages
#define WRD_PAGE_NOT_FOUND 0
#define WRD_PAGE_ROOT 1
#define WRD_PAGE_APP 2
#define WRD_PAGE_APP_KEYS 3
#define WRD_PAGE_APP_VIEWS 4

// Query string
#define WRD_DEFAULT_QUERY_LIMIT 10
#define WRD_DEFAULT_QUERY_MULTI 0

#define WRD_QUERY_VALUE(TYPE, NAME) short query_valid_##NAME; TYPE query_##NAME;
// #define WRD_QUERY_VALUES(TYPE, NAME, COUNT) short query_valid_##NAME = 0; TYPE query_##NAME[COUNT];

// Init the system
wrd_code wrd_init(const char *filename, warudo **config);

// Accept an HTTP connection
wrd_code wrd_accept_connection(warudo *config);

// Run at the end of a connection
wrd_code wrd_after_connection(warudo *config);

// Homepage error page
wrd_code wrd_page_home(warudo *config);

// Root JSON endpoints
wrd_code wrd_page_app(int entry_type, warudo *config);

// Keys JSON endpoint
wrd_code wrd_page_app_keys(warudo *config);

// Close the system
wrd_code wrd_close(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // SERVER_H