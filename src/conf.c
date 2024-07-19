
#include <ctype.h>
#include <string.h>

#include "conf.h"
#include "db.h"
#include "env.h"
#include "fs.h"

// This file automatically generated. Do not edit it manually.

// Init a configuration file with environment variables.
WRD_API wrd_code wrd_init_config(warudo *config) {
    // Database
    wrd_get_env_string(&config->db_path, "WRD_DB_PATH", WRD_DEFAULT_DB_PATH);
    // Log level can be one of the following [0, 1, 2, 3]: no_log, info, error, debug
    wrd_get_env_int((int*)&config->log_level, "WRD_LOG_LEVEL", WRD_DEFAULT_LOG_LEVEL);
    // Net
    wrd_get_env_string(&config->access_origin, "WRD_ACCESS_ORIGIN", WRD_DEFAULT_ACCESS_ORIGIN);
    wrd_get_env_int((int*)&config->listen_backlog, "WRD_LISTEN_BACKLOG", WRD_DEFAULT_LISTEN_BACKLOG);
    wrd_get_env_int((int*)&config->max_columns, "WRD_MAX_COLUMNS", WRD_DEFAULT_MAX_COLUMNS);
    wrd_get_env_int((int*)&config->net_buffer_size, "WRD_NET_BUFFER_SIZE", WRD_DEFAULT_NET_BUFFER_SIZE);
    wrd_get_env_int((int*)&config->net_headers_buffer_size, "WRD_NET_HEADERS_BUFFER_SIZE", WRD_DEFAULT_NET_HEADERS_BUFFER_SIZE);
    wrd_get_env_int((int*)&config->net_input_buffer_size, "WRD_NET_INPUT_BUFFER_SIZE", WRD_DEFAULT_NET_INPUT_BUFFER_SIZE);
    wrd_get_env_int((int*)&config->socket_port, "WRD_SOCKET_PORT", WRD_DEFAULT_SOCKET_PORT);
    wrd_get_env_int((int*)&config->timing, "WRD_TIMING", WRD_DEFAULT_TIMING);

    return WRD_OK;
}

// Close loaded configurations.
WRD_API wrd_code wrd_config_close(warudo *config) {
    if(config->access_origin) {
        free(config->access_origin);
        config->access_origin = NULL;
    }

    return WRD_OK;
}

// Load a configuration file.
WRD_API wrd_code wrd_load_config(warudo *config, const char *file_path) {
    wrd_config_close(config);
    wrd_init_config(config);

    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    void *file_buffer = NULL;
    wrd_code ret;
    long file_size;

    if(file_path == NULL) {
        // Nothing to load, return.
        return WRD_DEFAULT;
    }

    ret = wrd_read_file(file_path, &file_buffer, &file_size);

    if(ret != WRD_OK) {
        return ret;
    }

    if(file_size == 0) {
        // Empty file, return.
        return WRD_DEFAULT;
    }

    // Set an error by default.
    ret = WRD_DB_ERROR;
    // Initialize SQLite.
    int rc = sqlite3_initialize();

    if(rc != SQLITE_OK) {
        return WRD_DB_INIT_ERROR;
    }

    // Open in-memory database.
    rc = sqlite3_open(":memory:", &db);

    if(rc != SQLITE_OK) {
        return WRD_DB_OPEN_ERROR;
    }

    // Create a table to store the JSON data.
    const char *create_table = "CREATE TABLE json_data (data BLOB);";
    rc = sqlite3_exec(db, create_table, NULL, NULL, NULL);

    if(rc != SQLITE_OK) {
        goto error;
    }

    const char *load_json = "INSERT INTO json_data (data) VALUES (jsonb(?1));";
    rc = sqlite3_prepare_v2(db, load_json, -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        goto error;
    }

    // Bind the JSON data to the statement.
    rc = sqlite3_bind_blob(stmt, 1, file_buffer, file_size, SQLITE_STATIC);

    if(rc != SQLITE_OK) {
        goto error;
    }

    // Execute the statement
    rc = sqlite3_step(stmt);

    if(rc != SQLITE_DONE) {
        goto error;
    }

    sqlite3_finalize(stmt);
    stmt = NULL;

    const char *select_json = "SELECT data ->> '$.access_origin' FROM json_data;";
    rc = sqlite3_prepare_v2(db, select_json, -1, &stmt, 0);

    if(rc != SQLITE_OK) {
        goto error;
    }

    rc = sqlite3_step(stmt);

    if(rc != SQLITE_ROW) {
        goto error;
    }

    const char *access_origin = (const char*)sqlite3_column_text(stmt, 0);

    if(access_origin) {
        int length = sqlite3_column_bytes(stmt, 0);

        if(length) {
            config->access_origin = strndup(access_origin, length);
        }
    }
    puts(access_origin);

    ret = WRD_OK;

error:

    if(stmt) {
        sqlite3_finalize(stmt);
    }

    if(db) {
        sqlite3_close(db);
    }

    if(file_buffer) {
        free(file_buffer);
    }

    return ret;
}
