#ifndef WRD_CONF_H
#define WRD_CONF_H

// This file is generated by `make generate_config`. Do not edit it manually.

#ifdef __cplusplus
extern "C" {
#endif

// Database
#define WRD_DEFAULT_DB_PATH "file:warudo.db"

// Net
#define WRD_DEFAULT_LISTEN_BACKLOG 1024
#define WRD_DEFAULT_MAX_COLUMNS 64
#define WRD_DEFAULT_NET_BUFFER_SIZE 1
#define WRD_DEFAULT_NET_HEADERS_BUFFER_SIZE 4096
#define WRD_DEFAULT_NET_INPUT_BUFFER_SIZE 1
#define WRD_DEFAULT_SOCKET_PORT 6251

typedef struct wrd_config {
    // Database
    char *db_path;
    // Net
    int listen_backlog;
    int max_columns;
    int net_buffer_size;
    int net_headers_buffer_size;
    int net_input_buffer_size;
    int socket_port;
} wrd_config;

// Load a configuration file.
void wrd_init_config(wrd_config *config);

// Load a configuration file.
void wrd_load_config(wrd_config *config, const char *input);

#ifdef __cplusplus
}
#endif

#endif // WRD_CONF_H