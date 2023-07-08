#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <fcgiapp.h>

#include "warudo.h"
#include "html.h"

/*static void print_env(const char *label, char **envp, warudo *config) {
    FCGX_FPrintF(config->request.out, "%s:<br>\n<pre>\n", label);
    config->script_name = NULL;
    config->query_string = NULL;

    for(; *envp != NULL; envp++) {
        FCGX_FPrintF(config->request.out, "%s\n", *envp);
    }

    FCGX_FPrintF(config->request.out, "</pre>\n");
}*/

int warudo_page_home(warudo* config) {
    if(config->request_method != WARUDO_REQUEST_GET) {
        return warudo_not_allowed("GET", config);
    }

    /*FCGX_PutS("Status: 200 OK\r\n", config->request.out);
    warudo_content_type("text/html", config);

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
        "<script src=\"static/js/chart-" WARUDO_CHART_VERSION ".min.js\"></script>"
        "<script src=\"static/js/main.js\"></script>", config->request.out);
    // print_env("Environ", config->request.envp, config);
    warudo_environ(config);
    FCGX_PutS("</body></html>", config->request.out);

    return 0;*/
    return warudo_server_error("Zaerl is a Fast CGI application. "
        "You need to use a webserver and proxy the requests to the socket. "
        "Follow the instructions found in README.md.", config);
}
