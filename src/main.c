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
        } else if(config->page == ZAERL_PAGE_ROOT) {
            zaerl_page_home(config, count);
        } else {
            zaerl_page_not_found(config);
        }

        ++count;
    }

    zaerl_close(config);

    return 0;
}
