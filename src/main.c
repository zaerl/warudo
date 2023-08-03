#define NO_FCGI_DEFINES

#include <stdio.h>
#include "sqlite3/sqlite3.h"

#include "config.h"
#include "db.h"
#include "log.h"
#include "net.h"

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

// /opt/homebrew/opt/nginx/bin/nginx -g "daemon off;"
// /opt/homebrew/opt/lighttpd/bin/lighttpd -D -f /opt/homebrew/etc/lighttpd/lighttpd.conf
// make && WARUDO_CORS="*" WARUDO_LOG_LEVEL=3 bin/warudo
int main(void) {
    warudo *config;
    int ret;

    // warudo_read_config();
    ret = warudo_init(WARUDO_DB_FILE, &config);

    if(ret != WARUDO_OK) {
        warudo_log_error(config, "Failed to initialize\n", "");
        return ret;
    }

    while(warudo_accept_connection(config) >= 0) {
        /*char** envp = config->request.envp;

        while(*envp) {
            warudo_log_debug(config, "%s\n", *envp);
            ++envp;
        }*/
        if(config->page == WARUDO_PAGE_APP) {
            ret = page_app(WARUDO_ENTRY_TYPE_DATA, config);
        } else if(config->page == WARUDO_PAGE_APP_KEYS) {
            ret = page_app_keys(config);
        } else if(config->page == WARUDO_PAGE_APP_VIEWS) {
            ret = page_app(WARUDO_ENTRY_TYPE_VIEW, config);
        } else if(config->page == WARUDO_PAGE_ROOT) {
            ret = warudo_page_home(config);
        } else {
            ret = warudo_page_not_found(config);
        }

        warudo_after_connection(config);

        if(ret == 0) {
            warudo_log_info(config, "Accepted request %llu\n", config->requests_count);
        } else {
            warudo_log_error(config, "Failed to accept request %llu. Code %d\n", config->requests_count, ret);
        }
    }

    warudo_close(config);

    return WARUDO_OK;
}
