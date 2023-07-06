// #define NO_FCGI_DEFINES

#include <fcgiapp.h>
#include <stdio.h>
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

    zaerl_read_config();
    ret = zaerl_init(ZAERL_DB_FILE, &config);

    if (ret != 0) {
        return ret;
    }

    printf("Starting zaerl %s\n", ZAERL_VERSION);

    while(zaerl_accept_connection(config) >= 0) {
        if(config->page == ZAERL_PAGE_APP) {
            ret = page_app(config);
        } else if(config->page == ZAERL_PAGE_ROOT) {
            ret = zaerl_page_home(config);
        } else {
            ret = zaerl_page_not_found(config);
        }

        if(ret == 0) {
            fprintf(stderr, "Accepted request %llu\n", config->requests_count);
        } else {
            fprintf(stderr, "Failed to accept request %llu. Code %d\n", config->requests_count, ret);
        }

        ++config->requests_count;
    }

    zaerl_close(config);

    return 0;
}
