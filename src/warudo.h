/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */


#ifndef WARUDO_H
#define WARUDO_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#error "Windows is not supported"
#endif

#include <netinet/in.h>
#include <stdio.h>
#include <time.h>

#include "sqlite3/sqlite3.h"

#ifndef WRD_API
#ifdef WRD_TEST
// # define WRD_API __attribute__((weak))
# define WRD_API
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

#ifndef WRD_MAX_CONFIG_SIZE
// Maximum size of the configuration file (512 kylobyte).
#define WRD_MAX_CONFIG_SIZE 524288
#endif

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
    WRD_PID_FILE,
    WRD_WORKER_PROCESSES,
} wrd_config_name;

// Database
#define WRD_DEFAULT_DB_PATH "file:warudo.db?hctree=1"

typedef enum {
    WRD_LOG_LEVEL_NO_LOG,
    WRD_LOG_LEVEL_INFO,
    WRD_LOG_LEVEL_ERROR,
    WRD_LOG_LEVEL_DEBUG
} wrd_log_level;

#define WRD_DEFAULT_LOG_LEVEL WRD_LOG_LEVEL_NO_LOG

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
#define WRD_DEFAULT_PID_FILE "/var/run/warudo.pid"
#define WRD_DEFAULT_WORKER_PROCESSES "auto"

// Database
#define WRD_DASHBOARDS_TABLE "dashboards"
#define WRD_ENTRIES_TABLE "entries"
#define WRD_AUTH_TABLE "http_auth"

#define WRD_ENTRY_TYPE_DATA 0
#define WRD_ENTRY_TYPE_VIEW 1

// Log
#define wrd_log_info(config, format, ...) wrd_log(config, WRD_LOG_LEVEL_INFO, format, __VA_ARGS__)
#define wrd_log_error(config, format, ...) wrd_log(config, WRD_LOG_LEVEL_ERROR, format, __VA_ARGS__)
#define wrd_log_debug(config, format, ...) wrd_log(config, WRD_LOG_LEVEL_DEBUG, format, __VA_ARGS__)

// Query
#define WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, FN, LENGTH_1) if(!config->query.valid_##IDX && \
    strncmp(KEY, #IDX, LENGTH_1) == 0) { \
    config->query.##IDX = FN; \
    config->query.valid_##IDX = 1; }
#define WRD_GET_QUERY_ULLINT_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strtoll(VALUE, NULL, 10), LENGTH_1)
#define WRD_GET_QUERY_INT_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strtol(VALUE, NULL, 10), LENGTH_1)
#define WRD_GET_QUERY_STRING_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strdup(VALUE), LENGTH_1)

#define WRD_FREE_QUERY_ULLINT_VALUE(NAME, DEFAULT_VALUE) config->query.##NAME = DEFAULT_VALUE; config->query.valid_##NAME = 0;
#define WRD_FREE_QUERY_INT_VALUE(NAME, DEFAULT_VALUE) config->query.##NAME = DEFAULT_VALUE; config->query.valid_##NAME = 0;
#define WRD_FREE_QUERY_STRING_VALUE(NAME) if(config->query.##NAME != NULL) free((void*)config->query.##NAME); \
    config->query.##NAME = NULL; \
    config->query.valid_##NAME = 0;

// Routes
#define WRD_REQUEST_UNKNOWN 0
#define WRD_REQUEST_GET 1
#define WRD_REQUEST_POST 2

#define WRD_PAGE_NOT_FOUND 0
#define WRD_PAGE_ROOT 1
#define WRD_PAGE_APP 2
#define WRD_PAGE_APP_KEYS 3
#define WRD_PAGE_APP_VIEWS 4

#define WRD_REQUEST_HEAD 3
#define WRD_REQUEST_PUT 4
#define WRD_REQUEST_DELETE 5
#define WRD_REQUEST_OPTIONS 6

#define WRD_DEFAULT_QUERY_LIMIT 10
#define WRD_DEFAULT_QUERY_MULTI 0

// HTTP request headers
#define WRD_MAX_HEADERS 64
#define WRD_MAX_HEADER_NAME 128
#define WRD_MAX_HEADER_VALUE 4096
#define WRD_MAX_PATH 2048

typedef struct wrd_http_header_entry {
    char name[WRD_MAX_HEADER_NAME];
    char value[WRD_MAX_HEADER_VALUE];
} wrd_http_header_entry;

#define WRD_QUERY_VALUE(TYPE, NAME) short query_valid_##NAME; TYPE query_##NAME;

typedef enum {
    WRD_DEFAULT_CONFIG = 'D',
    WRD_LOADED_CONFIG = 'C',
    WRD_ENV_CONFIG = 'E',
} wrd_config_status;

typedef enum {
    // Success codes
    WRD_OK = 0,
    WRD_DEFAULT = 1,

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

typedef enum {
    WRD_NOT_LOADED = 0,
    WRD_LOADED = 1,
} wrd_status;

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
    wrd_status status;
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

    char config_status[12];

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
    char *pid_file;
    char *worker_processes;

    // Network
    int server_fd;
    int client_fd;
    struct sockaddr_in address;

    // Worker
    int is_worker;
    long num_workers;
    wrd_worker *workers;

    // Buffers
    wrd_buffer net_headers_buffer;
    wrd_buffer net_buffer;
    wrd_buffer net_input_buffer;

    // HTTP request (parsed from input buffer).
    int page;
    int request_method;
    char request_path[WRD_MAX_PATH];
    const char *script_name;
    const char *query_string;
    char request_query_string[WRD_MAX_PATH];
    int http_version_major;
    int http_version_minor;
    wrd_http_header_entry request_headers[WRD_MAX_HEADERS];
    int request_headers_count;
    unsigned int request_body_offset;
    wrd_column columns[WRD_DEFAULT_MAX_COLUMNS];
    unsigned int columns_count;
    unsigned long long int requests_count;
    struct timespec start;
    unsigned long long int timing_count;
    unsigned long long int timing_end_count;

    // Status.
    wrd_status status;

    // Query string
    wrd_query query;
    wrd_query valid_query;
} warudo;

// Auth
WRD_API char* wrd_base64_encode(const char *input);
WRD_API char* wrd_base64_decode(const char *input);

// Code
WRD_API const char *wrd_error_description(wrd_code code, int extended);

WRD_API wrd_code wrd_config_init(warudo *config, const char *file_path);
WRD_API wrd_code wrd_config_init_defaults(warudo *config);
WRD_API wrd_code wrd_load_config_env(warudo *config);
WRD_API wrd_code wrd_config_close(warudo *config);
WRD_API wrd_config_status wrd_get_config_status(warudo *config, wrd_config_name name);
WRD_API wrd_code wrd_set_config_status(warudo *config, wrd_config_name name, wrd_config_status status);

// Data
size_t wrd_debug_dump(const char* text, FILE* stream, unsigned char* ptr, size_t size);

WRD_API wrd_code wrd_db_init(const char *filename, warudo *config);
WRD_API wrd_code wrd_db_close(warudo *config);
WRD_API wrd_code wrd_db_add_header(warudo *config, const char *name, const char *value);
unsigned long long wrd_last_insert_rowid(warudo *config);
WRD_API wrd_code wrd_parse_json(warudo *config);
WRD_API wrd_code wrd_add_index(const char *filename, warudo *config);
WRD_API wrd_code wrd_add_entry(int entry_type, warudo *config);
WRD_API wrd_code wrd_formdata_callback(const char *input, long int length, warudo *config);
WRD_API wrd_code wrd_add_entries(int entry_type, warudo *config);
WRD_API wrd_code wrd_get_entries(int entry_type, warudo *config);
WRD_API wrd_code wrd_get_keys(warudo *config);

// Env
WRD_API wrd_code wrd_get_env_int(int *result, const char *name);
WRD_API wrd_code wrd_get_env_string(char **result, const char *name);

// Filesystem
WRD_API wrd_code wrd_read_file(const char *file_path, void **file_buffer,
    long *file_size, long max_size);

// HPACK
WRD_API wrd_code wrd_compress_header(const char *name, const char *value, char *output);
WRD_API wrd_code wrd_decompress_header(const char *input, char *name, char *value);

// HTTP
WRD_API wrd_code wrd_http_status(warudo *config, const char *status);
WRD_API wrd_code wrd_http_content_type(warudo *config, const char *content_type);
WRD_API wrd_code wrd_http_header(warudo *config, const char *status, const char *content_type);
WRD_API wrd_code wrd_http_ok(warudo *config);
WRD_API wrd_code wrd_http_created(warudo *config, unsigned long long int id);
WRD_API wrd_code wrd_http_multi_created(warudo *config, unsigned long int count);
WRD_API wrd_code wrd_http_not_allowed(warudo *config, const char *allowed);
WRD_API wrd_code wrd_http_server_error(warudo *config, const char *description);
WRD_API wrd_code wrd_http_bad_request(warudo *config, const char *description);
WRD_API wrd_code wrd_http_not_found(warudo *config);
WRD_API wrd_code wrd_http_puts(warudo *config, const char *str);
WRD_API wrd_code wrd_http_printf(warudo *config, const char *format, ...);
WRD_API wrd_code wrd_http_parse_query_headers(warudo *config);
WRD_API wrd_code wrd_http_get_header(warudo *config, const char *name, char **value);
WRD_API wrd_code wrd_http_flush(warudo *config);

// Log
WRD_API wrd_code wrd_log(warudo *config, wrd_log_level level, const char *format, ...);

// Net
WRD_API wrd_code wrd_net_init(warudo *config, int backlog);
WRD_API wrd_code wrd_net_close(warudo *config);
WRD_API wrd_code wrd_net_accept(warudo *config);
WRD_API wrd_code wrd_net_finish_request(warudo *config);
WRD_API wrd_code wrd_net_printf(warudo *config, wrd_buffer *buffer, const char *format, ...);
WRD_API wrd_code wrd_net_read(warudo *config);
WRD_API wrd_code wrd_net_send(warudo *config, wrd_buffer *buffer);

// Output
WRD_API char *wrd_escape_html(const char *input);

typedef int (*wrd_parse_formdata_callback)(const char*, long int, warudo*);

WRD_API wrd_code wrd_parse_query_string(warudo *config, char *query_string);
WRD_API char *wrd_url_decode(const char *input);
WRD_API int wrd_is_valid_boundary(const char *boundary);
WRD_API const char *wrd_get_formdata_boundary(const char *content_type);
WRD_API wrd_code wrd_parse_formdata(const char *input, long int length, const char *boundary,
    wrd_parse_formdata_callback callback, warudo *config);
WRD_API long wrd_content_length(warudo *config);
WRD_API char *wrd_read_content(long int length, warudo *config);

WRD_API wrd_code wrd_route_home(warudo *config);
WRD_API wrd_code wrd_route_app(int entry_type, warudo *config);
WRD_API wrd_code wrd_route_app_keys(warudo *config);

// Server
WRD_API wrd_code wrd_server_init(warudo *config);
WRD_API wrd_code wrd_server_save_pid(warudo *config);
WRD_API wrd_code wrd_server_loop(warudo *config);
WRD_API wrd_code wrd_server_close(warudo *config);

// Timing
WRD_API wrd_code wrd_start_time(warudo *config);
WRD_API wrd_code wrd_end_time(warudo *config, const char *message);

// TLS
WRD_API wrd_code wrd_init_tls(warudo *config);

// Version
WRD_API int wrd_version(warudo *config);

// Worker
WRD_API wrd_code wrd_worker_init(warudo *config);
WRD_API wrd_code wrd_get_workers(warudo *config, long *workers);
WRD_API wrd_code wrd_worker_close(warudo *config);
WRD_API wrd_code wrd_worker_loop(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WARUDO_H
