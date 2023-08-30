#ifndef WARUDO_H
#define WARUDO_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#error "Windows is not supported"
#endif

#ifndef USE_LIBFCGI
#define USE_LIBFCGI 1
#endif

#include <fcgiapp.h>
#include "sqlite3/sqlite3.h"

// Version
#define WRD_VERSION "0.1.0"

// Configurations
#define WRD_BUFFLEN 8192
#define WRD_CONFIG_FILE "warudo.conf"
#define WRD_DB_FILE "file:warudo.db"
#define WRD_HCTREE_DB_FILE "file:warudo.db?hctree=1"
#define WRD_ENTRIES_TABLE "entries"
#define WRD_MAX_COLUMNS 64
#define WRD_SOCKET_PATH ":6251"
#define WRD_TIMING 1
#define WRD_DASHBOARDS_TABLE "dashboards"
#define WRD_MAX_QUERY_KEYS 16

#ifdef WRD_TIMING
#include <time.h>
#endif

typedef enum {
    WRD_LOG_LEVEL_NO_LOG = 0,
    WRD_LOG_LEVEL_ERROR = 1,
    WRD_LOG_LEVEL_INFO = 2,
    WRD_LOG_LEVEL_DEBUG = 3
} wrd_log_level;

// Environment variables
#define WRD_DEFAULT_CORS NULL
#define WRD_DEFAULT_LOG_LEVEL WRD_LOG_LEVEL_NO_LOG

typedef enum {
    // Success code
    WRD_OK = 0,

    // HTTP codes
    WRD_HTTP_BAD_REQUEST = 400,
    WRD_HTTP_NOT_FOUND = 404,
    WRD_HTTP_NOT_ALLOWED = 405,
    WRD_HTTP_INTERNAL_ERROR = 500,

    // Error codes
    WRD_ERROR = -1,
    WRD_ACCEPT_ERROR = -2,
    WRD_ADD_ERROR = -3,
    WRD_DB_CLOSE_ERROR = -4,
    WRD_DB_ERROR = -5,
    WRD_DB_INIT_ERROR = -6,
    WRD_DB_OPEN_ERROR = -7,
    WRD_EMPTY_CONTENT_ERROR = -8,
    WRD_EMPTY_QUERY_STRING_ERROR = -9,
    WRD_FCGI_INIT_ERROR = -10,
    WRD_INIT_REQUEST_ERROR = -11,
    WRD_READ_ERROR = -12,
    WRD_SOCKET_ERROR = -13,

    // Parser error codes
    WRD_PARSER_EMPTY = -14,
    WRD_PARSER_EMPTY_BOUNDARY = -15,
    WRD_PARSER_VOID = -16,
    WRD_PARSER_NO_BOUNDARY = -17,
    WRD_PARSER_MISSING_END = -18,
    WRD_PARSER_MISSING_CONTENT = -19,
    WRD_PARSER_MISSING_BODY = -20,

    // Unknown error codes
    WRD_UNKNOWN_ERROR = -21
} wrd_code;

// HTTP request methods
#define WRD_REQUEST_UNKNOWN 0
#define WRD_REQUEST_GET 1
#define WRD_REQUEST_POST 2

// Pages
#define WRD_PAGE_NOT_FOUND 0
#define WRD_PAGE_ROOT 1
#define WRD_PAGE_APP 2
#define WRD_PAGE_APP_KEYS 3
#define WRD_PAGE_APP_VIEWS 4

// Tables
#define WRD_ENTRY_TYPE_DATA 0
#define WRD_ENTRY_TYPE_VIEW 1

// Query string
#define WRD_DEFAULT_QUERY_LIMIT 10
#define WRD_DEFAULT_QUERY_MULTI 0

#define WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, FN, LENGTH_1) if(!config->query_valid_##IDX && \
    strncmp(KEY, #IDX, LENGTH_1) == 0) { \
    config->query_##IDX = FN; \
    config->query_valid_##IDX = 1; }
#define WRD_GET_QUERY_ULLINT_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strtoll(VALUE, NULL, 10), LENGTH_1)
#define WRD_GET_QUERY_INT_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strtol(VALUE, NULL, 10), LENGTH_1)
#define WRD_GET_QUERY_STRING_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strdup(VALUE), LENGTH_1)

#define WRD_FREE_QUERY_ULLINT_VALUE(NAME, DEFAULT_VALUE) config->query_##NAME = DEFAULT_VALUE; config->query_valid_##NAME = 0;
#define WRD_FREE_QUERY_INT_VALUE(NAME, DEFAULT_VALUE) config->query_##NAME = DEFAULT_VALUE; config->query_valid_##NAME = 0;
#define WRD_FREE_QUERY_STRING_VALUE(NAME) if(config->query_##NAME != NULL) free((void*)config->query_##NAME); \
    config->query_##NAME = NULL; \
    config->query_valid_##NAME = 0;
// #define WRD_FREE_QUERY_STRING_VALUES(NAME) for(int i = 0; i < config->query_valid_##NAME; ++i) { free((void*)config->query_##NAME[i]); \
//  config->query_##NAME[i] = NULL; } \
//  config->query_valid_##NAME = 0;

#define WRD_QUERY_VALUE(TYPE, NAME) short query_valid_##NAME; TYPE query_##NAME;
// #define WRD_QUERY_VALUES(TYPE, NAME, COUNT) short query_valid_##NAME = 0; TYPE query_##NAME[COUNT];

#define CHECK_CONFIG if(!config) return WRD_ERROR;

struct wrd_column {
    char *name;
    char *type;
};

typedef struct wrd_column wrd_column;

typedef struct {
    sqlite3 *db;
    sqlite3_stmt *insert_stmt;
    sqlite3_stmt *insert_dashboard_stmt;
    sqlite3_stmt *add_index_stmt;
    sqlite3_stmt *parse_json_stmt;

    // Environment variables
    const char *access_origin;
    wrd_log_level log_level;

    FCGX_Request request;
    int socket;
    int page;
    int request_method;
    const char *script_name;
    const char *query_string;
    wrd_column columns[WRD_MAX_COLUMNS];
    unsigned int columns_count;
    unsigned long long int requests_count;
#ifdef WRD_TIMING
    struct timespec start;
    unsigned long long int timing_count;
    unsigned long long int timing_end_count;
#endif

    // Query string
    WRD_QUERY_VALUE(unsigned long long int, id)
    WRD_QUERY_VALUE(unsigned long long int, offset)
    WRD_QUERY_VALUE(unsigned int, limit)
    WRD_QUERY_VALUE(unsigned int, multi)
    WRD_QUERY_VALUE(const char*, key)
    WRD_QUERY_VALUE(const char*, value)
    WRD_QUERY_VALUE(const char*, orderby)
    WRD_QUERY_VALUE(const char*, sort)
    // WRD_QUERY_VALUES(const char*, keys, WRD_MAX_QUERY_KEYS)
    // WRD_QUERY_VALUES(const char*, values, WRD_MAX_QUERY_KEYS)
} warudo;

// Init the system
wrd_code wrd_init(const char *filename, warudo **config);

// Parse the query string
wrd_code wrd_parse_query_string(char *query_string, warudo *config);

// Accept a FCGI connection
wrd_code wrd_accept_connection(warudo *config);

// Run at the end of a connection
wrd_code wrd_after_connection(warudo *config);

// Homepage error page
wrd_code wrd_page_home(warudo *config);

// Root JSON endpoints
wrd_code wrd_page_app(int entry_type, warudo *config);

// Keys JSON endpoint
wrd_code wrd_page_app_keys(warudo *config);

// Timing functions
#ifdef WRD_TIMING
wrd_code wrd_start_time(warudo *config);

wrd_code wrd_end_time(warudo *config, double ms, const char *message);
#endif

// Close the system
wrd_code wrd_close(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WARUDO_H
