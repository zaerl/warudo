#ifndef ZAERL_H
#define ZAERL_H

#define ZAERL_VERSION "0.1.0"

#define ZAERL_DB_FILE "zaerl.db"
#define ZAERL_CONFIG_FILE "zaerl.conf"
#define ZAERL_SOCKET_PATH ":6251"

#include <fcgiapp.h>
#include "sqlite3.h"

#define ZAERL_PAGE_NOT_FOUND 0
#define ZAERL_PAGE_ROOT 1
#define ZAERL_PAGE_APP 2

#define ZAERL_REQUEST_UNKNOWN 0
#define ZAERL_REQUEST_GET 1
#define ZAERL_REQUEST_POST 2

struct zaerl {
    sqlite3 *db;
    FCGX_Request request;
    int socket;
    int page;
    int request_method;
    const char* script_name;
    const char* query_string;
};

typedef struct zaerl zaerl;

int zaerl_init(const char *filename, zaerl **config);

int zaerl_accept_connection(zaerl *config);

int page_home(zaerl* config, unsigned long count);

int page_app(zaerl* config, unsigned long count);

int zaerl_close(zaerl *config);

#endif /* ZAERL_H */
