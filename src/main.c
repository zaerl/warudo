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
// make && ./bin/warudo"
int main(void) {
    warudo *config;
    int ret;

    warudo_read_config();
    ret = warudo_init(WARUDO_DB_FILE, &config);

    if (ret != 0) {
        return ret;
    }

    printf("Starting warudo %s\n", WARUDO_VERSION);

    while(warudo_accept_connection(config) >= 0) {
        if(config->page == WARUDO_PAGE_APP) {
            ret = page_app("entries", config);
        } else if(config->page == WARUDO_PAGE_APP_KEYS) {
            ret = page_app_keys(config);
        } else if(config->page == WARUDO_PAGE_ROOT) {
            ret = warudo_page_home(config);
        } else {
            ret = warudo_page_not_found(config);
        }

        if(ret == 0) {
            fprintf(stderr, "Accepted request %llu\n", config->requests_count);
        } else {
            fprintf(stderr, "Failed to accept request %llu. Code %d\n", config->requests_count, ret);
        }

        ++config->requests_count;
    }

    warudo_close(config);

    return 0;
}
