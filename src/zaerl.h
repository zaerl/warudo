#ifndef ZAERL_H
#define ZAERL_H

#define ZAERL_VERSION "0.1.0"

#define ZAERL_DB_FILE "zaerl.db"
#define ZAERL_CONFIG_FILE "zaerl.conf"
#define ZAERL_SOCKET_PATH ":6251"
#define ZAERL_MAX_COLUMNS 64
#define ZAERL_LOG_LEVEL 1
#define ZAERL_CHART_VERSION "4.3.0"

#include <fcgiapp.h>
#include "sqlite3.h"

#define ZAERL_PAGE_NOT_FOUND 0
#define ZAERL_PAGE_ROOT 1
#define ZAERL_PAGE_APP 2
#define ZAERL_PAGE_APP_KEYS 3

#define ZAERL_REQUEST_UNKNOWN 0
#define ZAERL_REQUEST_GET 1
#define ZAERL_REQUEST_POST 2

// Query string
#define ZAERL_DEFAULT_QUERY_LIMIT 100

#define ZA_GET_QUERY_VALUE(KEY, IDX, VALUE, FN, LENGTH_1) if(!config->query_valid_##IDX && strncmp(KEY, #IDX, LENGTH_1) == 0) {\
    config->query_##IDX = FN; \
    config->query_valid_##IDX = 1; }
#define ZA_GET_QUERY_INT_VALUE(KEY, IDX, VALUE, LENGTH_1) ZA_GET_QUERY_VALUE(KEY, IDX, VALUE, strtol(VALUE, NULL, 10), LENGTH_1)
#define ZA_GET_QUERY_STRING_VALUE(KEY, IDX, VALUE, LENGTH_1) ZA_GET_QUERY_VALUE(KEY, IDX, VALUE, strdup(VALUE), LENGTH_1)

#define ZA_FREE_QUERY_INT_VALUE(NAME, DEFAULT_VALUE) config->query_##NAME = DEFAULT_VALUE; config->query_valid_##NAME = 0;
#define ZA_FREE_QUERY_STRING_VALUE(NAME) if(config->query_##NAME != NULL) free((void*)config->query_##NAME); config->query_##NAME = NULL; config->query_valid_##NAME = 0;

#define ZA_QUERY_VALUE(TYPE, NAME) short query_valid_##NAME; TYPE query_##NAME;

struct zaerl_column {
    char* name;
    char* type;
};

typedef struct zaerl_column zaerl_column;

struct zaerl {
    sqlite3 *db;
    FCGX_Request request;
    int socket;
    int page;
    int request_method;
    const char* script_name;
    const char* query_string;
    zaerl_column columns[ZAERL_MAX_COLUMNS];
    unsigned int columns_count;
    unsigned long long int requests_count;

    // Query string
    ZA_QUERY_VALUE(short, limit)
    ZA_QUERY_VALUE(const char*, key)
    ZA_QUERY_VALUE(const char*, value)
};

typedef struct zaerl zaerl;

int zaerl_init(const char *filename, zaerl **config);

int zaerl_accept_connection(zaerl *config);

int zaerl_page_home(zaerl* config);

int page_app(zaerl* config);

int page_app_keys(zaerl* config);

int zaerl_close(zaerl *config);

#endif /* ZAERL_H */
