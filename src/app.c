#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zaerl.h"
#include "html.h"

int page_app(zaerl* config, unsigned long count) {
    if(!(config->request_method == ZAERL_REQUEST_GET || config->request_method == ZAERL_REQUEST_POST)) {
        return zaerl_not_allowed("GET, POST", config);
    }

    FCGX_PutS("Status: 200 OK\r\n", config->request.out);
    zaerl_content_type("application/json", config);

    printf("Accepted request %lu\n", count);

    return FCGX_PutS("{}\n", config->request.out);
}
