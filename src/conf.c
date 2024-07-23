
#include <ctype.h>
#include <string.h>

#include "conf.h"
#include "db.h"
#include "env.h"
#include "fs.h"

// Init configurations with default values.
WRD_API wrd_code wrd_init_config(warudo *config) {
    config->columns_count = 0;
    config->requests_count = 0;
    config->timing_count = 0;
    config->timing_end_count = 0;

    // Configurations.

    // Initialize to defaults.
    memset(config->config_status, WRD_DEFAULT_CONFIG, 11);

    // Database
    config->db_path = WRD_DEFAULT_DB_PATH;
    config->log_level = WRD_DEFAULT_LOG_LEVEL;
    // Net
    config->access_origin = WRD_DEFAULT_ACCESS_ORIGIN;
    config->listen_backlog = WRD_DEFAULT_LISTEN_BACKLOG;
    config->max_columns = WRD_DEFAULT_MAX_COLUMNS;
    config->net_buffer_size = WRD_DEFAULT_NET_BUFFER_SIZE;
    config->net_headers_buffer_size = WRD_DEFAULT_NET_HEADERS_BUFFER_SIZE;
    config->net_input_buffer_size = WRD_DEFAULT_NET_INPUT_BUFFER_SIZE;
    config->socket_port = WRD_DEFAULT_SOCKET_PORT;
    config->timing = WRD_DEFAULT_TIMING;
    // Server
    config->workers = WRD_DEFAULT_WORKERS;

    return WRD_OK;
}

// Set configurations to environment variable values, if they exist.
WRD_API wrd_code wrd_load_config_env(warudo *config) {
    // Status of the loaded configuration.
    wrd_code ret;

    // Used to shorten the declarations.
    #define LOAD_ENV_CONFIG_INT(NAME, FIELD) \
        ret = wrd_get_env_int((int*)&config->FIELD, #NAME); \
        if(ret == WRD_OK) wrd_set_config_status(config, NAME, WRD_ENV_CONFIG);

    #define LOAD_ENV_CONFIG_STRING(NAME, FIELD) \
        ret = wrd_get_env_string(&config->FIELD, #NAME); \
        if(ret == WRD_OK) wrd_set_config_status(config, NAME, WRD_ENV_CONFIG);

    // Configurations.

    // Database
    LOAD_ENV_CONFIG_STRING(WRD_DB_PATH, db_path)
    LOAD_ENV_CONFIG_INT(WRD_LOG_LEVEL, log_level)
    // Net
    LOAD_ENV_CONFIG_STRING(WRD_ACCESS_ORIGIN, access_origin)
    LOAD_ENV_CONFIG_INT(WRD_LISTEN_BACKLOG, listen_backlog)
    LOAD_ENV_CONFIG_INT(WRD_MAX_COLUMNS, max_columns)
    LOAD_ENV_CONFIG_INT(WRD_NET_BUFFER_SIZE, net_buffer_size)
    LOAD_ENV_CONFIG_INT(WRD_NET_HEADERS_BUFFER_SIZE, net_headers_buffer_size)
    LOAD_ENV_CONFIG_INT(WRD_NET_INPUT_BUFFER_SIZE, net_input_buffer_size)
    LOAD_ENV_CONFIG_INT(WRD_SOCKET_PORT, socket_port)
    LOAD_ENV_CONFIG_INT(WRD_TIMING, timing)
    // Server
    LOAD_ENV_CONFIG_STRING(WRD_WORKERS, workers)

    return WRD_OK;
}

// Close loaded configurations.
WRD_API wrd_code wrd_config_close(warudo *config) {
    // Used to shorten the declarations.
    #define FREE_CONFIG_STRING(NAME, FIELD) \
        if(config->FIELD && wrd_get_config_status(config, NAME) != WRD_DEFAULT_CONFIG) { \
            free(config->FIELD); \
            config->FIELD = NULL; \
        }

    // Configurations.

    // Database
    FREE_CONFIG_STRING(WRD_DB_PATH, db_path)

    // Net
    FREE_CONFIG_STRING(WRD_ACCESS_ORIGIN, access_origin)

    // Server
    FREE_CONFIG_STRING(WRD_WORKERS, workers)

    return WRD_OK;
}

WRD_API wrd_config_status wrd_load_string(sqlite3_stmt *stmt, const char *name, char **output) {
    wrd_config_status ret = WRD_DEFAULT_CONFIG;

    if(sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC) != SQLITE_OK) {
        goto error;
    }

    if(sqlite3_step(stmt) != SQLITE_ROW) {
        goto error;
    }

    if(sqlite3_column_type(stmt, 0) == SQLITE_TEXT) {
        const char *res = (const char*)sqlite3_column_text(stmt, 0);

        if(!res) {
            goto error;
        }

        int length = sqlite3_column_bytes(stmt, 0);

        if(length) {
            *output = strndup(res, length);
            ret = WRD_LOADED_CONFIG;
        }
    }

error:

    sqlite3_reset(stmt);

    return ret;
}

WRD_API wrd_config_status wrd_load_integer(sqlite3_stmt *stmt, const char *name, int *output) {
    wrd_config_status ret = WRD_DEFAULT_CONFIG;

    if(sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC) != SQLITE_OK) {
        goto error;
    }

    if(sqlite3_step(stmt) != SQLITE_ROW) {
        goto error;
    }

    if(sqlite3_column_type(stmt, 0) == SQLITE_INTEGER) {
        *output = sqlite3_column_int(stmt, 0);
        ret = WRD_LOADED_CONFIG;
    }

error:

    sqlite3_reset(stmt);

    return ret;
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
    const char *create_table = u8"CREATE TABLE json_data (data BLOB);";
    rc = sqlite3_exec(db, create_table, NULL, NULL, NULL);

    if(rc != SQLITE_OK) {
        goto error;
    }

    const char *load_json = u8"INSERT INTO json_data (data) VALUES (jsonb(?1));";
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

    const char *select_json = u8"SELECT data ->> ('$.' || ?1) FROM json_data;";
    rc = sqlite3_prepare_v2(db, select_json, -1, &stmt, 0);

    if(rc != SQLITE_OK) {
        goto error;
    }

    // Status of the new configurations.
    wrd_config_status status;

    // Used to shorten the declarations.
    #define LOAD_DB_CONFIG_INT(NAME, FIELD) \
        status = wrd_load_integer(stmt, #FIELD, (int*)&config->FIELD); \
        wrd_set_config_status(config, NAME, status);

    #define LOAD_DB_CONFIG_STR(NAME, FIELD) \
        status = wrd_load_string(stmt, #FIELD, &config->FIELD); \
        wrd_set_config_status(config, NAME, status);

    // Configurations.

    LOAD_DB_CONFIG_STR(WRD_DB_PATH, db_path)
    LOAD_DB_CONFIG_INT(WRD_LOG_LEVEL, log_level)
    LOAD_DB_CONFIG_STR(WRD_ACCESS_ORIGIN, access_origin)
    LOAD_DB_CONFIG_INT(WRD_LISTEN_BACKLOG, listen_backlog)
    LOAD_DB_CONFIG_INT(WRD_MAX_COLUMNS, max_columns)
    LOAD_DB_CONFIG_INT(WRD_NET_BUFFER_SIZE, net_buffer_size)
    LOAD_DB_CONFIG_INT(WRD_NET_HEADERS_BUFFER_SIZE, net_headers_buffer_size)
    LOAD_DB_CONFIG_INT(WRD_NET_INPUT_BUFFER_SIZE, net_input_buffer_size)
    LOAD_DB_CONFIG_INT(WRD_SOCKET_PORT, socket_port)
    LOAD_DB_CONFIG_INT(WRD_TIMING, timing)
    LOAD_DB_CONFIG_STR(WRD_WORKERS, workers)

    wrd_load_config_env(config);

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

// Get the status of a configuration.
WRD_API wrd_config_status wrd_get_config_status(warudo *config, wrd_config_name name) {
    if(!config) {
        return WRD_NOT_LOADED_CONFIG;
    }

    if(!config->config_status[name]) {
        return WRD_NOT_LOADED_CONFIG;
    }

    return config->config_status[name];
}

// Set the status of a configuration.
WRD_API wrd_code wrd_set_config_status(warudo *config, wrd_config_name name, wrd_config_status status) {
    CHECK_CONFIG

    config->config_status[name] = status;

    return WRD_OK;
}
