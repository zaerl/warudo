#ifndef WARUDO_H
#define WARUDO_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#error "Windows is not supported"
#endif

#ifndef WRD_API
#ifdef WRD_TEST
# define WRD_API __attribute__((weak))
#else
# define WRD_API
#endif
#endif

#define WRD_NAME u8"Warudo"
#define WRD_VERSION_MAJOR 0
#define WRD_VERSION_MINOR 1
#define WRD_VERSION_RELEASE 0
#define WRD_VERSION u8"0.1.0"
#define WRD_VERSION_NUMBER 1000

// Headers
#define WRD_AUTH_DEFAULT_USER "admin"
#define WRD_AUTH_DEFAULT_PASS "admin"
#define WRD_AUTH_DEFAULT_ROLE "admin"

#define WRD_DEFAULT_CONF_PATH "warudo.conf"

// Configurations.
// Configuration names.
typedef enum {
    WRD_DB_PATH,
    WRD_LOG_LEVEL,
    WRD_ACCESS_ORIGIN,
    WRD_LISTEN_BACKLOG,
    WRD_MAX_COLUMNS,
    WRD_NET_BUFFER_SIZE,
    WRD_NET_HEADERS_BUFFER_SIZE,
    WRD_NET_INPUT_BUFFER_SIZE,
    WRD_SOCKET_PORT,
    WRD_TIMING,
    WRD_WORKER_PROCESSES,
} wrd_config_name;

// Database
#define WRD_DEFAULT_DB_PATH "file:warudo.db"
typedef enum {
    WRD_LOG_LEVEL_NO_LOG,
    WRD_LOG_LEVEL_INFO,
    WRD_LOG_LEVEL_ERROR,
    WRD_LOG_LEVEL_DEBUG
} wrd_log_level;

#define WRD_DEFAULT_LOG_LEVEL 0

// Net
#define WRD_DEFAULT_ACCESS_ORIGIN NULL
#define WRD_DEFAULT_LISTEN_BACKLOG 1024
#define WRD_DEFAULT_MAX_COLUMNS 64
#define WRD_DEFAULT_NET_BUFFER_SIZE 1
#define WRD_DEFAULT_NET_HEADERS_BUFFER_SIZE 4096
#define WRD_DEFAULT_NET_INPUT_BUFFER_SIZE 1
#define WRD_DEFAULT_SOCKET_PORT 6251
#define WRD_DEFAULT_TIMING 1

// Server
#define WRD_DEFAULT_WORKER_PROCESSES "auto"

#include <netinet/in.h>
#include <time.h>

#include "sqlite3/sqlite3.h"

typedef enum {
    // Success codes
    WRD_OK = 0,
    WRD_DEFAULT = 1,
    WRD_LOADED = 2,

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
    WRD_FILE_ERROR = -11,
    WRD_MEMORY_ERROR = -12,
    WRD_MISSING_CRYPTO_ERROR = -13,
    WRD_INVALID_CONFIG = -14,

    // Parser error codes
    WRD_PARSER_EMPTY = -32,
    WRD_PARSER_EMPTY_BOUNDARY = -33,
    WRD_PARSER_VOID = -34,
    WRD_PARSER_NO_BOUNDARY = -35,
    WRD_PARSER_MISSING_END = -36,
    WRD_PARSER_MISSING_CONTENT = -37,
    WRD_PARSER_MISSING_BODY = -38,

    // Network error codes
    WRD_SOCKET_ERROR = -64,
    WRD_BIND_ERROR = -65,
    WRD_LISTEN_ERROR = -66,
    WRD_ACCEPT_ERROR = -67,
    WRD_CLOSE_ERROR = -68,
    WRD_SETSOCKOPT_ERROR = -69,

    // Unknown error codes
    WRD_UNKNOWN_ERROR = -80
} wrd_code;

#define CHECK_CONFIG if(!config || config->status != WRD_LOADED) return WRD_ERROR;

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

typedef struct wrd_worker {
    pid_t pid;
    int status;
} wrd_worker;

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

    // Configurations.

    char config_status[11];

    // Database
    char *db_path;
    wrd_log_level log_level;
    // Net
    char *access_origin;
    int listen_backlog;
    int max_columns;
    int net_buffer_size;
    int net_headers_buffer_size;
    int net_input_buffer_size;
    int socket_port;
    int timing;
    // Server
    char *worker_processes;

    // Network
    int server_fd;
    int client_fd;
    struct sockaddr_in address;

    // Worker
    int is_worker;
    wrd_worker *workers;

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
    wrd_code status;

    // Query string
    wrd_query query;
    wrd_query valid_query;
} warudo;

#ifdef __cplusplus
}
#endif

#endif // WARUDO_H
