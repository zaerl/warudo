#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zaerl.h"
#include "html.h"

int page_app(zaerl* config, unsigned long count) {
    FCGX_PutS("Status: 200 OK\r\n", config->request.out);

    printf("Accepted request %lu\n", count);
    zaerl_content_type("application/json", config);

    return FCGX_PutS("{}\n", config->request.out);
}
