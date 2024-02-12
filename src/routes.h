#ifndef ROUTES_H
#define ROUTES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "warudo.h"

// Homepage error page
wrd_code wrd_route_home(warudo *config);

// Root JSON endpoints
wrd_code wrd_route_app(int entry_type, warudo *config);

// Keys JSON endpoint
wrd_code wrd_route_app_keys(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // ROUTES_H
