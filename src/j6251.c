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
        printf("Content-type: text/html\r\n\r\n");
        printf("<html>\n");
        printf("<head><title>Hello</title></head>\n");
        printf("<body>\n");
        printf("<p>DB: %s</p>\n", ZAERL_DB_FILE);
        printf("<p>Config: %s</p>\n", ZAERL_CONFIG_FILE);
        printf("</body>\n");
        printf("</html>\n");
    }

    zaerl_close(db);

    return 0;
}
