#include "http.h"
#include "routes.h"

WRD_API wrd_code wrd_route_home(warudo *config) {
    CHECK_CONFIG

    return wrd_http_server_error(config, "Silence is golden");
}
