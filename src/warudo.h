#ifndef WARUDO_H
#define WARUDO_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#error "Windows is not supported"
#endif

// Version
#define WRD_NAME "Warudo"
#define WRD_VERSION "0.1.0"

// Configurations
#define WRD_BUFFLEN 8192
#define WRD_CONFIG_FILE "warudo.conf"
#define WRD_DB_FILE "file:warudo.db"
#define WRD_HCTREE_DB_FILE "file:warudo.db?hctree=1"
#define WRD_MAX_COLUMNS 64
// #define WRD_SOCKET_PATH ":6251"
#define WRD_SOCKET_PORT 6251
#define WRD_NET_HEADERS_BUFFER_SIZE 4096 // In bytes
#define WRD_NET_BUFFER_SIZE 1 // In MB
#define WRD_NET_INPUT_BUFFER_SIZE 1 // In MB
#define WRD_LISTEN_BACKLOG 1024 // TODO: 1024?
#define WRD_TIMING 1
#define WRD_MAX_QUERY_KEYS 16

// Default tables
#define WRD_DASHBOARDS_TABLE "dashboards"
#define WRD_ENTRIES_TABLE "entries"
#define WRD_AUTH_TABLE "http_auth"

// Headers
#define WRD_AUTH_DEFAULT_USER "admin"
#define WRD_AUTH_DEFAULT_PASS "admin"
#define WRD_AUTH_DEFAULT_ROLE "admin"

#ifdef WRD_TIMING
#include <time.h>
#endif

#include <netinet/in.h>

#include "sqlite3/sqlite3.h"

// Tables
#define WRD_ENTRY_TYPE_DATA 0
#define WRD_ENTRY_TYPE_VIEW 1

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

struct wrd_column {
    char *name;
    char *type;
};

typedef struct wrd_column wrd_column;

struct wrd_buffer {
    char *buffer;
    unsigned int size;
    unsigned int position;
};

typedef struct wrd_buffer wrd_buffer;

struct wrd_query {
    unsigned long long int id;
    unsigned long long int offset;
    unsigned int limit;
    unsigned int multi;
    const char *key;
    const char *value;
    const char *orderby;
    const char *sort;
};

typedef struct wrd_query wrd_query;

typedef struct {
    sqlite3 *db;
    sqlite3_stmt *insert_stmt;
    sqlite3_stmt *insert_dashboard_stmt;
    sqlite3_stmt *add_index_stmt;
    sqlite3_stmt *parse_json_stmt;

    // Environment variables
    const char *access_origin;
    wrd_log_level log_level;

    // Network
    int server_fd;
    int client_fd;
    struct sockaddr_in address;

    wrd_buffer net_headers_buffer;
    wrd_buffer net_buffer;
    wrd_buffer net_input_buffer;

    int socket; // REMOVE
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
    wrd_query query;
    wrd_query valid_query;
} warudo;

#ifdef __cplusplus
}
#endif

#endif // WARUDO_H
