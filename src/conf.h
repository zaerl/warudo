#ifndef WRD_CONF_H
#define WRD_CONF_H

// This file automatically generated. Do not edit it manually.

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WRD_API
#define WRD_API
#endif

// Database
#define WRD_DEFAULT_DB_PATH "file:warudo.db"

// Log level can be one of the following [0, 1, 2, 3]: no_log, info, error, debug
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

typedef struct wrd_config {
    // Database
    char *db_path;
    // Log level can be one of the following [0, 1, 2, 3]: no_log, info, error, debug
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
} wrd_config;

// Load a configuration file.
WRD_API void wrd_init_config(wrd_config *config);

// Load a configuration file.
WRD_API int wrd_load_config(wrd_config *config, const char *file_path);

#ifdef __cplusplus
}
#endif

#endif // WRD_CONF_H
