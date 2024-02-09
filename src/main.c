#define NO_FCGI_DEFINES

#include <stdio.h>
#include "sqlite3/sqlite3.h"

#include "code.h"
#include "db.h"
#include "http.h"
#include "log.h"
#include "output.h"

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

// /opt/homebrew/opt/nginx/bin/nginx -g "daemon off;"
// /opt/homebrew/opt/lighttpd/bin/lighttpd -D -f /opt/homebrew/etc/lighttpd/lighttpd.conf
// make && WARUDO_CORS="*" WARUDO_LOG_LEVEL=3 bin/warudo.cgi
int main(void) {
    warudo *config = NULL;
    wrd_code ret;

    ret = wrd_init(WRD_DB_FILE, &config);

    if(ret != WRD_OK) {
        wrd_log_error(config, "Failed to initialize\n", "");

        return ret;
    }

    while(wrd_accept_connection(config) == WRD_OK) {
        wrd_after_connection(config);
    }

    /*
    while(wrd_accept_connection(config) >= 0) {
        /*char **envp = config->request.envp;

        while(*envp) {
            wrd_log_debug(config, "%s\n", *envp);
            ++envp;
        }*
        if(config->page == WRD_PAGE_APP) {
            ret = wrd_page_app(WRD_ENTRY_TYPE_DATA, config);
        } else if(config->page == WRD_PAGE_APP_KEYS) {
            ret = wrd_page_app_keys(config);
        } else if(config->page == WRD_PAGE_APP_VIEWS) {
            ret = wrd_page_app(WRD_ENTRY_TYPE_VIEW, config);
        } else if(config->page == WRD_PAGE_ROOT) {
            ret = wrd_page_home(config);
        } else {
            ret = wrd_http_not_found(config);
        }

        wrd_after_connection(config);

        if(ret == 0) {
            wrd_log_info(config, "Accepted request %llu\n", config->requests_count);
        } else {
            wrd_log_error(config, "Failed to accept request %llu. Code %s\n",
                config->requests_count, wrd_error_description(ret, 0));
        }
    }*/
    wrd_log_info(config, "Bye %s!\n", WRD_VERSION);
    wrd_close(config);

    return WRD_OK;
}
