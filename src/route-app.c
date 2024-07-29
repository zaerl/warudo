#include "db.h"
#include "http.h"
#include "server.h"
#include "routes.h"

WRD_API wrd_code wrd_route_app(int entry_type, warudo *config) {
    CHECK_CONFIG

    if(config->request_method == WRD_REQUEST_GET) {
        return wrd_get_entries(entry_type, config);
    } else if(config->request_method == WRD_REQUEST_POST) {
        if(config->query.multi && entry_type == WRD_ENTRY_TYPE_DATA) {
            return wrd_add_entries(entry_type, config);
        } else {
            return wrd_add_entry(entry_type, config);
        }
    }

    return wrd_http_not_allowed(config, "GET, POST");
}

WRD_API wrd_code wrd_route_app_keys(warudo *config) {
    CHECK_CONFIG

    if(config->request_method == WRD_REQUEST_GET) {
        return wrd_get_keys(config);
    }

    return wrd_http_not_allowed(config, "GET");
}
