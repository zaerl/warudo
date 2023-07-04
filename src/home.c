#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zaerl.h"
#include "html.h"

static void print_env(const char *label, char **envp, zaerl *config) {
    FCGX_FPrintF(config->request.out, "%s:<br>\n<pre>\n", label);
    config->script_name = NULL;
    config->query_string = NULL;

    for(; *envp != NULL; envp++) {
        FCGX_FPrintF(config->request.out, "%s\n", *envp);
    }

    FCGX_FPrintF(config->request.out, "</pre>\n");
}

int zaerl_page_home(zaerl* config, unsigned long count) {
    if(config->request_method != ZAERL_REQUEST_GET) {
        return zaerl_not_allowed("GET", config);
    }

    FCGX_PutS("Status: 200 OK\r\n", config->request.out);
    zaerl_content_type("text/html", config);

    printf("Accepted request %lu\n", count);

    FCGX_FPrintF(config->request.out, "<html>\n"
        "<head><title>Zaerl</title></head>\n"
        "<body>\n");
    print_env("Environ", config->request.envp, config);
    zaerl_environ(config);

    return FCGX_FPrintF(config->request.out, "</body></html>\n");
}
