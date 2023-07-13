#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <fcgiapp.h>

#include "warudo.h"
#include "net.h"
#include "db.h"

int page_app(int entry_type, warudo* config) {
    if(config->request_method == WARUDO_REQUEST_GET) {
        warudo_header("200 OK", "application/json", config);

        return warudo_get_entries(entry_type, config);
    } else if(config->request_method == WARUDO_REQUEST_POST) {
        int res = 0;
        if(config->query_multi && entry_type == WARUDO_ENTRY_TYPE_DATA) {
            res = warudo_add_entries(entry_type, config);
        } else {
            res = warudo_add_entry(entry_type, config);
        }

        if(res != WARUDO_OK) {
            warudo_bad_request("Failed to add entries.", config);

            return WARUDO_ADD_ERROR;
        }

        warudo_header("201 Created", "application/json", config);
        FCGX_FPrintF(config->request.out, "{\"status\":\"success\",\"id\":%lld}", warudo_last_insert_rowid(config));

        return WARUDO_OK;
    }

    return warudo_not_allowed("GET, POST", config);
}

int page_app_keys(warudo* config) {
    if(config->request_method == WARUDO_REQUEST_GET) {
        warudo_header("200 OK", "application/json", config);

        return warudo_get_keys(config);
    }

    return warudo_not_allowed("GET", config);
}
