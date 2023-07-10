#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <fcgiapp.h>

#include "warudo.h"
#include "html.h"
#include "db.h"

int page_app(const char* table, warudo* config) {
    if(config->request_method == WARUDO_REQUEST_GET) {
        warudo_header("200 OK", "application/json", config);

        return warudo_get_entries(table, config);
    } else if(config->request_method == WARUDO_REQUEST_POST) {
        char *length = FCGX_GetParam("CONTENT_LENGTH", config->request.envp);
        long int len = 0;

        if(length != NULL) {
            len = strtol(length, NULL, 10);
        }

        if (len <= 0) {
            warudo_bad_request("No data from standard input.", config);

            return 1;
        }

        char *data = malloc(len + 1);

        FCGX_GetStr(data, len, config->request.in);
        data[len] = '\0';

        if(warudo_add_entry(table, data, config) != 0) {
            warudo_bad_request("Failed to add entry.", config);

            return 1;
        }

        warudo_header("201 Created", "application/json", config);
        FCGX_FPrintF(config->request.out, "{\"status\":\"success\",\"id\":%lld}", sqlite3_last_insert_rowid(config->db));

        return 0;
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
