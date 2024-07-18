
#include <ctype.h>

#include "conf.h"
#include "db.h"
#include "env.h"

// This file automatically generated. Do not edit it manually.

// Init a configuration file with environment variables.
WRD_API void wrd_init_config(warudo *config) {
    // Database
    config->db_path = wrd_get_env_string("WRD_DB_PATH", WRD_DEFAULT_DB_PATH);
    // Log level can be one of the following [0, 1, 2, 3]: no_log, info, error, debug
    config->log_level = wrd_get_env_int("WRD_LOG_LEVEL", WRD_DEFAULT_LOG_LEVEL);
    // Net
    config->access_origin = wrd_get_env_string("WRD_ACCESS_ORIGIN", WRD_DEFAULT_ACCESS_ORIGIN);
    config->listen_backlog = wrd_get_env_int("WRD_LISTEN_BACKLOG", WRD_DEFAULT_LISTEN_BACKLOG);
    config->max_columns = wrd_get_env_int("WRD_MAX_COLUMNS", WRD_DEFAULT_MAX_COLUMNS);
    config->net_buffer_size = wrd_get_env_int("WRD_NET_BUFFER_SIZE", WRD_DEFAULT_NET_BUFFER_SIZE);
    config->net_headers_buffer_size = wrd_get_env_int("WRD_NET_HEADERS_BUFFER_SIZE", WRD_DEFAULT_NET_HEADERS_BUFFER_SIZE);
    config->net_input_buffer_size = wrd_get_env_int("WRD_NET_INPUT_BUFFER_SIZE", WRD_DEFAULT_NET_INPUT_BUFFER_SIZE);
    config->socket_port = wrd_get_env_int("WRD_SOCKET_PORT", WRD_DEFAULT_SOCKET_PORT);
    config->timing = wrd_get_env_int("WRD_TIMING", WRD_DEFAULT_TIMING);
}

// Load a configuration file.
WRD_API int wrd_load_config(warudo *config, const char *file_path) {
    wrd_init_config(config);

    if(file_path == NULL) {
        return WRD_ERROR;
    }

    int rc = sqlite3_initialize();

    if(rc != SQLITE_OK) {
        return WRD_DB_INIT_ERROR;
    }

    sqlite3 *db;
    sqlite3_stmt *stmt;

    rc = sqlite3_open(":memory", &db);

    if(rc != SQLITE_OK) {
        return WRD_DB_OPEN_ERROR;
    }

    const char *create_table = "CREATE TABLE json_data (data TEXT)";
    rc = sqlite3_exec(db, create_table, NULL, NULL, NULL);

    if(rc != SQLITE_OK) {
        sqlite3_close(db);

        return WRD_DB_ERROR;
    }

    const char *load_json = "INSERT INTO json_data (data) VALUES (readfile(?1))";
    rc = sqlite3_prepare_v2(db, load_json, -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        sqlite3_close(db);

        return WRD_DB_ERROR;
    }

    rc = sqlite3_bind_text(stmt, 1, file_path, -1, SQLITE_STATIC);

    // Execute the statement
    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return rc == SQLITE_DONE ? WRD_OK : WRD_DB_ERROR;
}
