#ifndef WARUDO_H
#define WARUDO_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#error "Windows is not supported"
#endif

#ifndef MJB_EXTERN
#define MJB_EXTERN extern
#endif

#ifndef MJB_EXPORT
#define MJB_EXPORT __attribute__((visibility("default")))
#endif

#define WARUDO_VERSION "0.1.0"

// Configurations
#define WARUDO_BUFFLEN 8192
#define WARUDO_CONFIG_FILE "warudo.conf"
#define WARUDO_DB_FILE "warudo.db"
#define WARUDO_ENTRIES_TABLE "entries"
#define WARUDO_MAX_COLUMNS 64
#define WARUDO_SOCKET_PATH ":6251"
#define WARUDO_TIMING 1
#define WARUDO_DASHBOARDS_TABLE "dashboards"
#define WARUDO_MAX_QUERY_KEYS 16

#define WARUDO_LOG_LEVEL_NO_LOG 0
#define WARUDO_LOG_LEVEL_ERROR 1
#define WARUDO_LOG_LEVEL_INFO 2
#define WARUDO_LOG_LEVEL_DEBUG 3

// Environment variables
#define WARUDO_DEFAULT_CORS NULL
#define WARUDO_DEFAULT_LOG_LEVEL WARUDO_LOG_LEVEL_NO_LOG

#ifdef WARUDO_TIMING
#include <time.h>
#endif

#include <fcgiapp.h>
#include "sqlite3/sqlite3.h"

// Success code
#define WARUDO_OK 0

// Error codes
#define WARUDO_ERROR 1
#define WARUDO_ACCEPT_ERROR 2
#define WARUDO_ADD_ERROR 3
#define WARUDO_DB_CLOSE_ERROR 4
#define WARUDO_DB_ERROR 5
#define WARUDO_DB_INIT_ERROR 6
#define WARUDO_DB_OPEN_ERROR 7
#define WARUDO_EMPTY_CONTENT_ERROR 8
#define WARUDO_EMPTY_QUERY_STRING_ERROR 9
#define WARUDO_FCGI_INIT_ERROR 10
#define WARUDO_INIT_REQUEST_ERROR 11
#define WARUDO_READ_ERROR 12
#define WARUDO_SOCKET_ERROR 13

// HTTP request methods
#define WARUDO_REQUEST_UNKNOWN 0
#define WARUDO_REQUEST_GET 1
#define WARUDO_REQUEST_POST 2

// HTTP codes
#define WARUDO_HTTP_BAD_REQUEST 400
#define WARUDO_HTTP_NOT_FOUND 404
#define WARUDO_HTTP_NOT_ALLOWED 405
#define WARUDO_HTTP_INTERNAL_ERROR 500

// Pages
#define WARUDO_PAGE_NOT_FOUND 0
#define WARUDO_PAGE_ROOT 1
#define WARUDO_PAGE_APP 2
#define WARUDO_PAGE_APP_KEYS 3
#define WARUDO_PAGE_APP_VIEWS 4

// Tables
#define WARUDO_ENTRY_TYPE_DATA 0
#define WARUDO_ENTRY_TYPE_VIEW 1

// Query string
#define WARUDO_DEFAULT_QUERY_LIMIT 100
#define WARUDO_DEFAULT_QUERY_MULTI 0

#define WARUDO_GET_QUERY_VALUE(KEY, IDX, VALUE, FN, LENGTH_1) if(!config->query_valid_##IDX && \
    strncmp(KEY, #IDX, LENGTH_1) == 0) { \
    config->query_##IDX = FN; \
    config->query_valid_##IDX = 1; }
#define WARUDO_GET_QUERY_ULLINT_VALUE(KEY, IDX, VALUE, LENGTH_1) WARUDO_GET_QUERY_VALUE(KEY, IDX, VALUE, strtoll(VALUE, NULL, 10), LENGTH_1)
#define WARUDO_GET_QUERY_INT_VALUE(KEY, IDX, VALUE, LENGTH_1) WARUDO_GET_QUERY_VALUE(KEY, IDX, VALUE, strtol(VALUE, NULL, 10), LENGTH_1)
#define WARUDO_GET_QUERY_STRING_VALUE(KEY, IDX, VALUE, LENGTH_1) WARUDO_GET_QUERY_VALUE(KEY, IDX, VALUE, strdup(VALUE), LENGTH_1)

#define WARUDO_FREE_QUERY_ULLINT_VALUE(NAME, DEFAULT_VALUE) config->query_##NAME = DEFAULT_VALUE; config->query_valid_##NAME = 0;
#define WARUDO_FREE_QUERY_INT_VALUE(NAME, DEFAULT_VALUE) config->query_##NAME = DEFAULT_VALUE; config->query_valid_##NAME = 0;
#define WARUDO_FREE_QUERY_STRING_VALUE(NAME) if(config->query_##NAME != NULL) free((void*)config->query_##NAME); \
    config->query_##NAME = NULL; \
    config->query_valid_##NAME = 0;
// #define WARUDO_FREE_QUERY_STRING_VALUES(NAME) for(int i = 0; i < config->query_valid_##NAME; ++i) { free((void*)config->query_##NAME[i]); \
//  config->query_##NAME[i] = NULL; } \
//  config->query_valid_##NAME = 0;

#define WARUDO_QUERY_VALUE(TYPE, NAME) short query_valid_##NAME; TYPE query_##NAME;
// #define WARUDO_QUERY_VALUES(TYPE, NAME, COUNT) short query_valid_##NAME = 0; TYPE query_##NAME[COUNT];

struct warudo_column {
    char* name;
    char* type;
};

typedef struct warudo_column warudo_column;

struct warudo {
    sqlite3 *db;
    sqlite3_stmt* insert_stmt;
    sqlite3_stmt* insert_dashboard_stmt;
    sqlite3_stmt* add_index_stmt;
    sqlite3_stmt* parse_json_stmt;

    // Environment variables
    const char* access_origin;
    int log_level;

    FCGX_Request request;
    int socket;
    int page;
    int request_method;
    const char* script_name;
    const char* query_string;
    warudo_column columns[WARUDO_MAX_COLUMNS];
    unsigned int columns_count;
    unsigned long long int requests_count;
#ifdef WARUDO_TIMING
    struct timespec start;
    unsigned long long int timing_count;
    unsigned long long int timing_end_count;
#endif

    // Query string
    WARUDO_QUERY_VALUE(unsigned long long int, id)
    WARUDO_QUERY_VALUE(unsigned long long int, offset)
    WARUDO_QUERY_VALUE(unsigned int, limit)
    WARUDO_QUERY_VALUE(unsigned int, multi)
    WARUDO_QUERY_VALUE(const char*, key)
    WARUDO_QUERY_VALUE(const char*, value)
    WARUDO_QUERY_VALUE(const char*, orderby)
    WARUDO_QUERY_VALUE(const char*, sort)
    // WARUDO_QUERY_VALUES(const char*, keys, WARUDO_MAX_QUERY_KEYS)
    // WARUDO_QUERY_VALUES(const char*, values, WARUDO_MAX_QUERY_KEYS)
};

typedef struct warudo warudo;

// Init the system
int warudo_init(const char *filename, warudo **config);

// Accept a FCGI connection
int warudo_accept_connection(warudo *config);

// Run at the end of a connection
int warudo_after_connection(warudo *config);

// Homepage error page
int warudo_page_home(warudo* config);

// Root JSON endpoints
int page_app(int entry_type, warudo* config);

// Keys JSON endpoint
int page_app_keys(warudo* config);

// Timing functions
#ifdef WARUDO_TIMING
int warudo_start_time(warudo *config);

int warudo_end_time(warudo *config, double ms, const char* message);
#endif

// Close the system
int warudo_close(warudo *config);

#ifdef __cplusplus
}  /* End of the 'extern "C"' block */
#endif

#endif /* WARUDO_H */
