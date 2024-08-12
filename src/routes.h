#ifndef ROUTES_H
#define ROUTES_H

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

// Homepage error page
WRD_API wrd_code wrd_route_home(warudo *config);

// Root JSON endpoints
WRD_API wrd_code wrd_route_app(int entry_type, warudo *config);

// Keys JSON endpoint
WRD_API wrd_code wrd_route_app_keys(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // ROUTES_H
