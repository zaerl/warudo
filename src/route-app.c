#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "db.h"
#include "http.h"
#include "net.h"
#include "warudo.h"

warudo_code warudo_page_app(int entry_type, warudo *config) {
    CHECK_CONFIG

    if(config->request_method == WARUDO_REQUEST_GET) {
        return warudo_get_entries(entry_type, config);
    } else if(config->request_method == WARUDO_REQUEST_POST) {
        if(config->query_multi && entry_type == WARUDO_ENTRY_TYPE_DATA) {
            return warudo_add_entries(entry_type, config);
        } else {
            return warudo_add_entry(entry_type, config);
        }
    }

    return warudo_not_allowed("GET, POST", config);
}

warudo_code warudo_page_app_keys(warudo *config) {
    CHECK_CONFIG

    if(config->request_method == WARUDO_REQUEST_GET) {
        return warudo_get_keys(config);
    }

    return warudo_not_allowed("GET", config);
}
