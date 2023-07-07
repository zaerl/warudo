#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <fcgiapp.h>

#include "zaerl.h"
#include "html.h"

/*static void print_env(const char *label, char **envp, zaerl *config) {
    FCGX_FPrintF(config->request.out, "%s:<br>\n<pre>\n", label);
    config->script_name = NULL;
    config->query_string = NULL;

    for(; *envp != NULL; envp++) {
        FCGX_FPrintF(config->request.out, "%s\n", *envp);
    }

    FCGX_FPrintF(config->request.out, "</pre>\n");
}*/

int zaerl_page_home(zaerl* config) {
    if(config->request_method != ZAERL_REQUEST_GET) {
        return zaerl_not_allowed("GET", config);
    }

    /*FCGX_PutS("Status: 200 OK\r\n", config->request.out);
    zaerl_content_type("text/html", config);

    FCGX_PutS("<html>\n"
        "<head>"
        "<title>Zaerl</title>"
        "<link rel=\"apple-touch-icon\" sizes=\"180x180\" href=\"static/images/apple-touch-icon.png\">"
        "<link rel=\"icon\" type=\"image/png\" sizes=\"32x32\" href=\"/static/images/favicon-32x32.png\">"
        "<link rel=\"icon\" type=\"image/png\" sizes=\"16x16\" href=\"/static/images/favicon-16x16.png\">"
        "<link rel=\"manifest\" href=\"/static/images/site.webmanifest\">"
        "<link rel=\"stylesheet\" type=\"text/css\" href=\"static/css/skelet.min.css\">"
        "<link rel=\"stylesheet\" type=\"text/css\" href=\"static/css/main.css\">"
        "</head>"
        "<body>"
        "<div><select id=\"main-keys\"></select></div>"
        "<div><canvas id=\"main\"></canvas></div>"
        "<script src=\"static/js/chart-" ZAERL_CHART_VERSION ".min.js\"></script>"
        "<script src=\"static/js/main.js\"></script>", config->request.out);
    // print_env("Environ", config->request.envp, config);
    zaerl_environ(config);
    FCGX_PutS("</body></html>", config->request.out);

    return 0;*/
    return zaerl_server_error("Zaerl is a Fast CGI application. "
        "You need to use a webserver and proxy the requests to the socket. "
        "Follow the instructions found in README.md.", config);
}
