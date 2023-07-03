#define NO_FCGI_DEFINES

#include <fcgi_stdio.h>
#include "sqlite3.h"

#include "config.h"
#include "db.h"
#include "html.h"

#define ZAERL_VERSION "0.1.0"

int main(void) {
    zaerl *db;
    int ret;

    zaerl_read_config();
    ret = zaerl_init(ZAERL_DB_FILE, &db);

    if (ret != 0) {
        return ret;
    }

    while (FCGI_Accept() >= 0) {
        zaerl_content_type("text/html");

        FCGI_printf("<html>\n");
        FCGI_printf("<head><title>Hello</title></head>\n");
        FCGI_printf("<body>\n");
        FCGI_printf("<p>DB: %s</p>\n", ZAERL_DB_FILE);
        FCGI_printf("<p>Config: %s</p>\n", ZAERL_CONFIG_FILE);
        FCGI_printf("</body>\n");
        zaerl_environ();
        FCGI_printf("</html>\n");
    }

    zaerl_close(db);

    return 0;
}
