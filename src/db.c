#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "warudo.h"
#include "db.h"
#include "net.h"
#include "log.h"

#define WARUDO_DB_RET_CALL(STMT, CALL, RET) \
    rc = CALL; \
    if(rc != RET) { \
        warudo_log_error(config, "Failed to execute db query: %d/%d %s\n", rc, RET, sqlite3_errmsg(config->db)); \
        if(must_finalize) sqlite3_finalize(STMT); \
        if(must_free) sqlite3_free((void*)query); \
        warudo_bad_request("Failed to get data.", config); \
        return WARUDO_DB_ERROR; \
    } \

#define WARUDO_DB_CALL(STMT, CALL) WARUDO_DB_RET_CALL(STMT, CALL, SQLITE_OK)

int warudo_load_columns(warudo* config);\

int warudo_db_init(const char *filename, warudo* config) {
    CHECK_CONFIG

    // Load database
    int must_free = 0;
    int must_finalize = 0;
    int rc;
    char* error_msg = 0;
    const char* query = NULL;

    rc = sqlite3_open(filename, &config->db);

    if(rc != SQLITE_OK) {
        warudo_log_error(config, "Can't open database: %s\n", sqlite3_errmsg(config->db));

        return WARUDO_DB_OPEN_ERROR;
    }

    const char *sql =
        "CREATE TABLE IF NOT EXISTS " WARUDO_ENTRIES_TABLE "("
        "id INTEGER PRIMARY KEY, "
        "created INTEGER DEFAULT (UNIXEPOCH()), "
        "modified INTEGER DEFAULT (UNIXEPOCH()), "
        "data TEXT NOT NULL);"
        "CREATE TABLE IF NOT EXISTS " WARUDO_DASHBOARDS_TABLE "("
        "id INTEGER PRIMARY KEY, "
        "created INTEGER DEFAULT (UNIXEPOCH()), "
        "modified INTEGER DEFAULT (UNIXEPOCH()), "
        "data TEXT NOT NULL);"
        "PRAGMA journal_mode = WAL;"
        "PRAGMA synchronous = NORMAL;"
        "PRAGMA busy_timeout = 5000;";
    rc = sqlite3_exec(config->db, sql, 0, 0, &error_msg);

    if(rc != SQLITE_OK) {
        warudo_log_error(config, "SQL error: %s\n", error_msg);

        return WARUDO_DB_OPEN_ERROR;
    }

    WARUDO_DB_CALL(config->insert_stmt, sqlite3_prepare_v2(config->db, "INSERT INTO " WARUDO_ENTRIES_TABLE "(data) VALUES(json(?1));", -1, &config->insert_stmt, NULL));
    WARUDO_DB_CALL(config->insert_dashboard_stmt, sqlite3_prepare_v2(config->db, "INSERT INTO " WARUDO_DASHBOARDS_TABLE "(data) VALUES(json(?1));", -1, &config->insert_dashboard_stmt, NULL));
    WARUDO_DB_CALL(config->add_index_stmt, sqlite3_prepare_v2(config->db, "ALTER TABLE " WARUDO_ENTRIES_TABLE " add column name TEXT as (json_extract(value, ?2));", -1, &config->add_index_stmt, NULL));
    WARUDO_DB_CALL(config->parse_json_stmt, sqlite3_prepare_v2(config->db, "SELECT * FROM json_each(?1) WHERE type='true';", -1, &config->parse_json_stmt, NULL));

    // return warudo_load_columns(config);
    return WARUDO_OK;
}

int warudo_db_close(warudo *config) {
    CHECK_CONFIG

    for(unsigned int i = 0; i < config->columns_count; ++i) {
        free(config->columns[i].name);
        free(config->columns[i].type);

        config->columns[i].name = NULL;
        config->columns[i].type = NULL;
    }

    config->columns_count = 0;

    sqlite3_finalize(config->insert_stmt);
    sqlite3_finalize(config->insert_dashboard_stmt);
    sqlite3_finalize(config->add_index_stmt);
    sqlite3_finalize(config->parse_json_stmt);

    return sqlite3_close(config->db) == SQLITE_OK ? WARUDO_OK : WARUDO_DB_CLOSE_ERROR;
}

int warudo_load_columns(warudo* config) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 1;
    int rc;
    char* query = "PRAGMA table_info('" WARUDO_ENTRIES_TABLE "');";
    sqlite3_stmt* stmt;

    WARUDO_DB_CALL(stmt, sqlite3_prepare_v2(config->db, query, -1, &stmt, NULL));

    warudo_log_info(config, "Column names for table '%s':\n", WARUDO_ENTRIES_TABLE);

    while(sqlite3_step(stmt) == SQLITE_ROW) {
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        const char* type = (const char*)sqlite3_column_text(stmt, 2);

        config->columns[config->columns_count].name = strdup((const char*)name);
        config->columns[config->columns_count].type = strdup((const char*)type);
        config->columns_count++;
    }

    for(unsigned int i = 0; i < config->columns_count; ++i) {
        warudo_log_info(config, "%s: %s\n", config->columns[i].name, config->columns[i].type);
    }

    sqlite3_finalize(stmt);

    return WARUDO_OK;
}

unsigned long long int warudo_last_insert_rowid(warudo *config) {
    CHECK_CONFIG

    return sqlite3_last_insert_rowid(config->db);
}

int warudo_parse_json(warudo* config) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 0;
    int rc;
    const char* query = NULL;
    sqlite3_stmt* stmt = config->parse_json_stmt;
    sqlite3_reset(stmt);

    long int length = warudo_content_length(config);

    if(length <= 0) {
        return WARUDO_EMPTY_CONTENT_ERROR;
    }

    char* json = warudo_read_content(config, length);

    if(json == NULL) {
        return WARUDO_READ_ERROR;
    }

    WARUDO_DB_CALL(stmt, sqlite3_bind_text(stmt, 1, json, length, SQLITE_STATIC));

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *key_name = (const char*)sqlite3_column_text(stmt, 0);

        if(strcmp(key_name, "key") == 0) {
            const char *value = (const char*)sqlite3_column_text(stmt, 2);

            if(strcmp(key_name, "string") == 0) {
                FCGX_PutS(value, config->request.out);
            }

            break;
        }
    }

    warudo_created(warudo_last_insert_rowid(config), config);
    free(json);

    return WARUDO_OK;
}

int warudo_add_index(const char *filename, warudo* config) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 0;
    int rc;
    const char* query = NULL;
    sqlite3_stmt* stmt = config->add_index_stmt;
    sqlite3_reset(stmt);

    long int length = warudo_content_length(config);

    if(length <= 0) {
        return WARUDO_EMPTY_CONTENT_ERROR;
    }

    char* json = warudo_read_content(config, length);

    if(json == NULL) {
        return WARUDO_READ_ERROR;
    }

    WARUDO_DB_CALL(stmt, sqlite3_bind_text(stmt, 1, json, length, SQLITE_STATIC));
    WARUDO_DB_RET_CALL(stmt, sqlite3_step(stmt), SQLITE_DONE);

    warudo_created(warudo_last_insert_rowid(config), config);
    free(json);

    return WARUDO_OK;
}

int warudo_add_entry(int entry_type, warudo *config) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 0;
    int rc;
    const char* query = NULL;
    sqlite3_stmt* stmt = entry_type == WARUDO_ENTRY_TYPE_DATA ? config->insert_stmt : config->insert_dashboard_stmt;
    sqlite3_reset(stmt);

    long int length = warudo_content_length(config);

    if(length <= 0) {
        return WARUDO_EMPTY_CONTENT_ERROR;
    }

    char* json = warudo_read_content(config, length);

    if(json == NULL) {
        return WARUDO_READ_ERROR;
    }

    WARUDO_DB_CALL(stmt, sqlite3_bind_text(stmt, 1, json, length, SQLITE_STATIC));
    WARUDO_DB_RET_CALL(stmt, sqlite3_step(stmt), SQLITE_DONE);

    warudo_created(warudo_last_insert_rowid(config), config);
    free(json);

    return WARUDO_OK;
}

int warudo_add_entries(int entry_type, warudo *config) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 0;
    int rc;
    const char* query = NULL;
    sqlite3_stmt* stmt = entry_type == WARUDO_ENTRY_TYPE_DATA ? config->insert_stmt : config->insert_dashboard_stmt;
    sqlite3_reset(stmt);

    long int length = warudo_content_length(config);

    if(length <= 0) {
        return WARUDO_EMPTY_CONTENT_ERROR;
    }

    char* input = warudo_read_content(config, length);

    if(input == NULL) {
        goto error;
    }

    char* boundary = warudo_get_formdata_boundary(FCGX_GetParam("CONTENT_TYPE", config->request.envp));

    if(boundary == NULL) {
        goto error;
    }

    /*
     * --{boundary}\r\n (2 + boundary_length + 2)
     * Content-Disposition: form-data; name=".+"\r\n (38 + name + 1 + 2)
     * \r\n (2)
     * .+\r\n (content + 2)
     * --{boundary}--\r\n (2 + boundary_length + 2 + 2)
     *
     * newlines = 10
     * content disposition = 38 + 1 + 1
     * {} = 2
     * -- = 2
     */
    long int boundary_length = strlen(boundary);

    if(length < boundary_length * 2 + 54 || input[0] != '-' || input[1] != '-') {
        return WARUDO_EMPTY_CONTENT_ERROR;
    }

    char* start = input + 2;
    char* position = strstr(input, boundary);

    if(position != start) {
        goto error;
    }

    start += boundary_length;
    position = strstr(start, "\r\nContent-Disposition: form-data; name=\"");

    if(position != start) {
        goto error;
    }

    start += 40;

    position = strstr(start, "\"\r\n\r\n");

    if(position == NULL) {
        goto error;
    }

    start = position + 5;

    position = strstr(start, boundary);

    if(position == NULL || position < start + 4) {
        goto error;
    }

    long int json_length = position - start;

    if(start[json_length - 4] != '\r' || start[json_length - 3] != '\n' ||
        start[json_length - 2] != '-' || start[json_length - 1] != '-') {
        goto error;
    }

    json_length -= 4;

    warudo_ok(config);
    FCGX_PutStr(start, json_length, config->request.out);
    FCGX_PutS(input, config->request.out);

    free(input);

    return WARUDO_OK;

error:
    if(input) {
        free(input);
    }

    return WARUDO_READ_ERROR;
}

int warudo_get_entries(int entry_type, warudo *config) {
    CHECK_CONFIG

    int must_free = 1;
    int must_finalize = 1;
    int rc;
    char *query;
    sqlite3_stmt *stmt;
    int count = 0;
    int limit_index = 0;
    int has_search = !config->query_id && config->query_key != NULL && config->query_value != NULL;
    const char* table_name = entry_type == WARUDO_ENTRY_TYPE_DATA ? "entries" : "dashboards";
    const char *order_by = config->query_orderby ? config->query_orderby : "id";
    const char *sort = config->query_sort ? config->query_sort : "ASC";

    /*
     *
     * Has search
     * SELECT id, created, modified, data FROM %q WHERE CAST(data ->> ?1 AS TEXT) = ?2 ORDER BY %q %q LIMIT ?3 OFFSET ?4;
     *
     * Has ID
     * SELECT id, created, modified, data FROM %q WHERE id = ?1;
     *
     * Regular
     * SELECT id, created, modified, data FROM %q ORDER BY %q %q LIMIT ?1 OFFSET ?2;
     */
    if(has_search) {
        query = sqlite3_mprintf("SELECT id, created, modified, data FROM %q WHERE CAST(data ->> ?1 AS TEXT) LIKE ?2 ORDER BY %q %q LIMIT ?3 OFFSET ?4;", table_name, order_by, sort);
        limit_index = 3;
    } else if(config->query_id) {
        query = sqlite3_mprintf("SELECT id, created, modified, data FROM %q WHERE id = ?1;", table_name);
    } else {
        query = sqlite3_mprintf("SELECT id, created, modified, data FROM %q ORDER BY %q %q LIMIT ?1 OFFSET ?2;", table_name, order_by, sort);
        limit_index = 1;
    }

    WARUDO_DB_CALL(stmt, sqlite3_prepare_v2(config->db, query, -1, &stmt, NULL));

    if(has_search) {
        char* query_key = warudo_url_decode(config->query_key);
        char* query_value = warudo_url_decode(config->query_value);

        WARUDO_DB_CALL(stmt, sqlite3_bind_text(stmt, 1, config->query_key, strlen(config->query_key), SQLITE_STATIC));
        WARUDO_DB_CALL(stmt, sqlite3_bind_text(stmt, 2, config->query_value, strlen(config->query_value), SQLITE_STATIC));

        free(query_key);
        free(query_value);
    } else if(config->query_id) {
        WARUDO_DB_CALL(stmt, sqlite3_bind_int64(stmt, 1, config->query_id));
    }

    if(limit_index) {
        WARUDO_DB_CALL(stmt, sqlite3_bind_int(stmt, limit_index, config->query_limit));
        WARUDO_DB_CALL(stmt, sqlite3_bind_int64(stmt, limit_index + 1, config->query_offset));
    }

    warudo_ok(config);
    FCGX_PutS("[", config->request.out);

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Retrieve the result
        sqlite3_int64 id = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 created = sqlite3_column_int64(stmt, 1);
        sqlite3_int64 modified = sqlite3_column_int64(stmt, 2);
        const char *data = (const char*)sqlite3_column_text(stmt, 3);

        if(count != 0) {
            FCGX_PutS(",", config->request.out);
        }

        FCGX_FPrintF(config->request.out, "{\"id\":%lld,\"created\":%lld,\"modified\":%lld,\"data\":%s}",
            id, created, modified, data ? data : "{}");
        ++count;
    }

    FCGX_PutS("]", config->request.out);
    sqlite3_finalize(stmt);

    if(must_free) {
        sqlite3_free(query);
    }

    return WARUDO_OK;
}

int warudo_get_keys(warudo *config) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 1;
    int rc;
    char *query = "SELECT key, COUNT(*) AS occurrence_count FROM " WARUDO_ENTRIES_TABLE ", json_tree(data) GROUP BY key;";
    int count = 0;
    sqlite3_stmt *stmt;

    WARUDO_DB_CALL(stmt, sqlite3_prepare_v2(config->db, query, -1, &stmt, NULL));

    warudo_ok(config);
    FCGX_PutS("[", config->request.out);

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Retrieve the result
        const char *key_name = (const char*)sqlite3_column_text(stmt, 0);
        sqlite3_int64 key_count = sqlite3_column_int64(stmt, 1);

        if(key_name == NULL) {
            continue;
        }

        if(count != 0) {
            FCGX_PutS(",", config->request.out);
        }

        FCGX_FPrintF(config->request.out, "{\"name\":\"%s\",\"value\":%lld}", key_name, key_count);
        ++count;
    }

    FCGX_PutS("]", config->request.out);
    sqlite3_finalize(stmt);

    return WARUDO_OK;
}

/*int warudo_add_index(const char* name, warudo *config) {
    char *query = "ALTER TABLE " WARUDO_ENTRIES_TABLE " ADD COLUMN ?1 TEXT;"
        "AS JSON_EXTRACT(data, '$.%q') AS %q;";
}*/
