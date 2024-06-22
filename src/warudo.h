#ifndef WARUDO_H
#define WARUDO_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#error "Windows is not supported"
#endif

#define WRD_NAME "Warudo"
#define WRD_VERSION_MAJOR 0
#define WRD_VERSION_MINOR 1
#define WRD_VERSION_RELEASE 0
#define WRD_VERSION "0.1.0"
#define WRD_VERSION_NUMBER 1000

#include "conf.h"

// Headers
#define WRD_AUTH_DEFAULT_USER "admin"
#define WRD_AUTH_DEFAULT_PASS "admin"
#define WRD_AUTH_DEFAULT_ROLE "admin"

#include <time.h>
#include <netinet/in.h>
#include <stddef.h>

#include "sqlite3/sqlite3.h"

// Tables
#define WRD_ENTRY_TYPE_DATA 0
#define WRD_ENTRY_TYPE_VIEW 1

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
    WRD_ADD_ERROR = -2,
    WRD_DB_CLOSE_ERROR = -3,
    WRD_DB_ERROR = -4,
    WRD_DB_INIT_ERROR = -5,
    WRD_DB_OPEN_ERROR = -6,
    WRD_EMPTY_CONTENT_ERROR = -7,
    WRD_EMPTY_QUERY_STRING_ERROR = -8,
    WRD_INIT_REQUEST_ERROR = -9,
    WRD_READ_ERROR = -10,

    // Parser error codes
    WRD_PARSER_EMPTY = -11,
    WRD_PARSER_EMPTY_BOUNDARY = -12,
    WRD_PARSER_VOID = -13,
    WRD_PARSER_NO_BOUNDARY = -14,
    WRD_PARSER_MISSING_END = -15,
    WRD_PARSER_MISSING_CONTENT = -16,
    WRD_PARSER_MISSING_BODY = -17,

    // Network error codes
    WRD_SOCKET_ERROR = -18,
    WRD_BIND_ERROR = -19,
    WRD_LISTEN_ERROR = -20,
    WRD_ACCEPT_ERROR = -21,
    WRD_CLOSE_ERROR = -22,

    // Unknown error codes
    WRD_UNKNOWN_ERROR = -23
} wrd_code;

#define CHECK_CONFIG if(!config) return WRD_ERROR;

typedef struct wrd_column {
    char *name;
    char *type;
} wrd_column;

typedef struct wrd_buffer {
    char *buffer;
    unsigned int size;
    unsigned int position;
} wrd_buffer;

typedef struct wrd_query {
    unsigned long long int id;
    unsigned long long int offset;
    unsigned int limit;
    unsigned int multi;
    const char *key;
    const char *value;
    const char *orderby;
    const char *sort;
} wrd_query;

// Main Warudo confuguration.
typedef struct warudo {
    // Main database.
    sqlite3 *db;
    sqlite3_stmt *insert_stmt;
    sqlite3_stmt *insert_dashboard_stmt;
    sqlite3_stmt *add_index_stmt;
    sqlite3_stmt *parse_json_stmt;

    // HTTP query in-memory database.
    sqlite3 *query_db;
    sqlite3_stmt *insert_query_stmt;

    // Configurations
    wrd_config config;

    // Network
    int server_fd;
    int client_fd;
    struct sockaddr_in address;

    // Buffers
    wrd_buffer net_headers_buffer;
    wrd_buffer net_buffer;
    wrd_buffer net_input_buffer;

    // HTTP various.
    int page;
    int request_method;
    const char *script_name;
    const char *query_string;
    wrd_column columns[WRD_DEFAULT_MAX_COLUMNS];
    unsigned int columns_count;
    unsigned long long int requests_count;
    struct timespec start;
    unsigned long long int timing_count;
    unsigned long long int timing_end_count;

    // Query string
    wrd_query query;
    wrd_query valid_query;
} warudo;

#ifdef __cplusplus
}
#endif

#endif // WARUDO_H
