#ifndef WARUDO_H
#define WARUDO_H

#define WARUDO_VERSION "0.1.0"

#define WARUDO_DB_FILE "warudo.db"
#define WARUDO_CONFIG_FILE "warudo.conf"
#define WARUDO_SOCKET_PATH ":6251"
#define WARUDO_MAX_COLUMNS 64
#define WARUDO_LOG_LEVEL 1
#define WARUDO_CORS NULL
#define WARUDO_ENTRIES_TABLE "entries"
#define WARUDO_VIEWS_TABLE "views"
#define WARUDO_TIMING 1

#ifdef WARUDO_TIMING
#include <time.h>
#endif

#include <fcgiapp.h>
#include "sqlite3.h"

#define WARUDO_PAGE_NOT_FOUND 0
#define WARUDO_PAGE_ROOT 1
#define WARUDO_PAGE_APP 2
#define WARUDO_PAGE_APP_KEYS 3
#define WARUDO_PAGE_APP_VIEWS 4

#define WARUDO_REQUEST_UNKNOWN 0
#define WARUDO_REQUEST_GET 1
#define WARUDO_REQUEST_POST 2

#define WARUDO_ENTRY_TYPE_DATA 0
#define WARUDO_ENTRY_TYPE_VIEW 1

// Query string
#define WARUDO_DEFAULT_QUERY_LIMIT 100
#define WARUDO_DEFAULT_QUERY_MULTI 0

#define ZA_GET_QUERY_VALUE(KEY, IDX, VALUE, FN, LENGTH_1) if(!config->query_valid_##IDX && strncmp(KEY, #IDX, LENGTH_1) == 0) {\
    config->query_##IDX = FN; \
    config->query_valid_##IDX = 1; }
#define ZA_GET_QUERY_ULLINT_VALUE(KEY, IDX, VALUE, LENGTH_1) ZA_GET_QUERY_VALUE(KEY, IDX, VALUE, strtoll(VALUE, NULL, 10), LENGTH_1)
#define ZA_GET_QUERY_INT_VALUE(KEY, IDX, VALUE, LENGTH_1) ZA_GET_QUERY_VALUE(KEY, IDX, VALUE, strtol(VALUE, NULL, 10), LENGTH_1)
#define ZA_GET_QUERY_STRING_VALUE(KEY, IDX, VALUE, LENGTH_1) ZA_GET_QUERY_VALUE(KEY, IDX, VALUE, strdup(VALUE), LENGTH_1)

#define ZA_FREE_QUERY_ULLINT_VALUE(NAME, DEFAULT_VALUE) config->query_##NAME = DEFAULT_VALUE; config->query_valid_##NAME = 0;
#define ZA_FREE_QUERY_INT_VALUE(NAME, DEFAULT_VALUE) config->query_##NAME = DEFAULT_VALUE; config->query_valid_##NAME = 0;
#define ZA_FREE_QUERY_STRING_VALUE(NAME) if(config->query_##NAME != NULL) free((void*)config->query_##NAME); config->query_##NAME = NULL; config->query_valid_##NAME = 0;

#define ZA_QUERY_VALUE(TYPE, NAME) short query_valid_##NAME; TYPE query_##NAME;

struct warudo_column {
    char* name;
    char* type;
};

typedef struct warudo_column warudo_column;

struct warudo {
    sqlite3 *db;
    FCGX_Request request;
    int socket;
    int page;
    int request_method;
    const char* script_name;
    const char* query_string;
    const char* access_origin;
    warudo_column columns[WARUDO_MAX_COLUMNS];
    unsigned int columns_count;
    unsigned long long int requests_count;
    struct timespec start;
#ifdef WARUDO_TIMING
    unsigned long long int timing_count;
    unsigned long long int timing_end_count;
    double time_taken;
#endif

    // Query string
    ZA_QUERY_VALUE(unsigned long long int, id)
    ZA_QUERY_VALUE(unsigned long long int, offset)
    ZA_QUERY_VALUE(unsigned int, limit)
    ZA_QUERY_VALUE(unsigned int, multi)
    ZA_QUERY_VALUE(const char*, key)
    ZA_QUERY_VALUE(const char*, value)
};

typedef struct warudo warudo;

int warudo_init(const char *filename, warudo **config);

int warudo_accept_connection(warudo *config);

int warudo_after_connection(warudo *config);

int warudo_page_home(warudo* config);

int page_app(int entry_type, warudo* config);

int page_app_keys(warudo* config);

#ifdef WARUDO_TIMING
int warudo_start_time(warudo *config);

int warudo_end_time(warudo *config, double ms, const char* message);
#endif

int warudo_close(warudo *config);

#endif /* WARUDO_H */
