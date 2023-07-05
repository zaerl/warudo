#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <fcgiapp.h>

#include "zaerl.h"
#include "html.h"
#include "db.h"

int page_app(zaerl* config, unsigned long count) {
    if(config->request_method == ZAERL_REQUEST_GET) {
        FCGX_PutS("Status: 200 OK\r\n", config->request.out);
        zaerl_content_type("application/json", config);

        fprintf(stderr, "Accepted request %lu, cl: %s\n", count, FCGX_GetParam("CONTENT_LENGTH", config->request.envp));
        FCGX_PutS("{\"status\":\"success\"}\n", config->request.out);

        return 0;
    } else if(config->request_method == ZAERL_REQUEST_POST) {
        char *length = FCGX_GetParam("CONTENT_LENGTH", config->request.envp);
        long int len = 0;

        if(length != NULL) {
            len = strtol(length, NULL, 10);
        }

        if (len <= 0) {
            zaerl_bad_request("No data from standard input.", config);

            return 1;
        }

        char *data = malloc(len + 1);

        FCGX_GetStr(data, len, config->request.in);

        if(zaerl_add_entry(data, config) != 0) {
            zaerl_bad_request("Failed to add entry.", config);

            return 1;
        }

        FCGX_PutS("Status: 201 Created\r\n", config->request.out);
        zaerl_content_type("application/json", config);
        FCGX_FPrintF(config->request.out, "{\"status\":\"success\",\"id\":%lld}", sqlite3_last_insert_rowid(config->db));

        return 0;
    }

    zaerl_not_allowed("GET, POST", config);

    return 1;
}
