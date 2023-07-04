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

    FCGX_FPrintF(config->request.out, "</pre><p>\n");
}

int page_home(zaerl* config, unsigned long count) {
    FCGX_PutS("Status: 200 OK\r\n", config->request.out);

    printf("Accepted request %lu\n", count);
    zaerl_content_type("text/html", config);

    FCGX_FPrintF(config->request.out, "<html>\n");
    FCGX_FPrintF(config->request.out, "<head><title>Zaerl</title></head>\n");
    FCGX_FPrintF(config->request.out, "<body>\n");
    FCGX_FPrintF(config->request.out, "</body>\n");
    zaerl_environ(config);
    print_env("Environ", config->request.envp, config);
    FCGX_FPrintF(config->request.out, "</html>\n");
}
