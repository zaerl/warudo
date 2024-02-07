#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "db.h"
#include "http.h"
#include "output.h"
#include "warudo.h"

wrd_code wrd_page_app(int entry_type, warudo *config) {
    CHECK_CONFIG

    if(config->request_method == WRD_REQUEST_GET) {
        return wrd_get_entries(entry_type, config);
    } else if(config->request_method == WRD_REQUEST_POST) {
        if(config->query_multi && entry_type == WRD_ENTRY_TYPE_DATA) {
            return wrd_add_entries(entry_type, config);
        } else {
            return wrd_add_entry(entry_type, config);
        }
    }

    return wrd_not_allowed("GET, POST", config);
}

wrd_code wrd_page_app_keys(warudo *config) {
    CHECK_CONFIG

    if(config->request_method == WRD_REQUEST_GET) {
        return wrd_get_keys(config);
    }

    return wrd_not_allowed("GET", config);
}
