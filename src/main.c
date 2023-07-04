#define NO_FCGI_DEFINES

#include <fcgiapp.h>
#include <fcgi_stdio.h>
#include "sqlite3.h"

#include "config.h"
#include "db.h"
#include "html.h"

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

// /opt/homebrew/opt/nginx/bin/nginx -g "daemon off;"
// make && ./bin/zaerl"
int main(void) {
    zaerl *config;
    int ret;
    unsigned long count = 0;

    zaerl_read_config();
    ret = zaerl_init(ZAERL_DB_FILE, &config);

    if (ret != 0) {
        return ret;
    }

    printf("Starting zaerl %s\n", ZAERL_VERSION);

    while(zaerl_accept_connection(config) >= 0) {
        if(config->page == ZAERL_PAGE_APP) {
            page_app(config, count);
            ++count;

            continue;
        } else if(config->page == ZAERL_PAGE_ROOT) {
            zaerl_page_home(config, count);
            ++count;

            continue;
        } else {
            zaerl_page_not_found(config);
            ++count;

            continue;
        }

        /*FCGX_PutS("Status: 200 OK\r\n", config->request.out);

        printf("Accepted request %lu\n", count++);
        zaerl_content_type("text/html", config);

        FCGX_FPrintF(config->request.out, "<html>\n");
        FCGX_FPrintF(config->request.out, "<head><title>Zaerl</title></head>\n");
        FCGX_FPrintF(config->request.out, "<body>\n");
        FCGX_FPrintF(config->request.out, "</body>\n");
        zaerl_environ(config);
        print_env("Environ", config->request.envp, config);
        FCGX_FPrintF(config->request.out, "</html>\n");*/
    }

    zaerl_close(config);
    // FCGX_Free(&request, 1);

    return 0;
}
