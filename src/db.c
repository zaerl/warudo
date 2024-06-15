#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"
#include "http.h"
#include "log.h"
#include "query.h"

#define WRD_DB_RET_CALL(STMT, CALL, RET) \
    rc = CALL; \
    if(rc != RET) { \
        wrd_log_error(config, "Failed to execute db query: %d/%d %s (%s, %d)\n", rc, RET, sqlite3_errmsg(config->db), __FUNCTION__, __LINE__); \
        if(must_finalize) sqlite3_finalize(STMT); \
        if(must_free) sqlite3_free((void*)query); \
        if(must_output_error) wrd_http_bad_request(config, "Failed to get data."); \
        return WRD_DB_ERROR; \
    } \

#define WRD_DB_CALL(STMT, CALL) WRD_DB_RET_CALL(STMT, CALL, SQLITE_OK)

// wrd_code wrd_load_columns(warudo *config);
wrd_code wrd_db_query_init(warudo *config);

wrd_code wrd_db_init(const char *filename, warudo *config) {
    CHECK_CONFIG

    // Load database
    int must_free = 0;
    int must_finalize = 0;
    int must_output_error = 1;
    int rc;
    char *error_msg = 0;
    const char *query = NULL;

    rc = sqlite3_initialize();

    if(rc != SQLITE_OK) {
        wrd_log(config, WRD_LOG_LEVEL_ERROR, "Can't initialize DB\n");

        return WRD_DB_INIT_ERROR;
    }

    rc = sqlite3_open(filename, &config->db);

    if(rc != SQLITE_OK) {
        wrd_log_error(config, "Can't open database: %s\n", sqlite3_errmsg(config->db));

        return WRD_DB_OPEN_ERROR;
    }

    sqlite3_extended_result_codes(config->db, 1);

    const char *sql =
        "CREATE TABLE IF NOT EXISTS " WRD_ENTRIES_TABLE "("
        "id INTEGER PRIMARY KEY, "
        "created INTEGER DEFAULT (UNIXEPOCH()), "
        "modified INTEGER DEFAULT (UNIXEPOCH()), "
        "data BLOB NOT NULL);"

        "CREATE TABLE IF NOT EXISTS " WRD_DASHBOARDS_TABLE "("
        "id INTEGER PRIMARY KEY, "
        "created INTEGER DEFAULT (UNIXEPOCH()), "
        "modified INTEGER DEFAULT (UNIXEPOCH()), "
        "data BLOB NOT NULL);"

        "CREATE TABLE IF NOT EXISTS " WRD_AUTH_TABLE "("
        "id INTEGER PRIMARY KEY, "
        "username TEXT NOT NULL,"
        "password TEXT NOT NULL,"
        "user_role TEXT NOT NULL,"
        "created INTEGER DEFAULT (UNIXEPOCH()), "
        "modified INTEGER DEFAULT (UNIXEPOCH()), "
        "data BLOB NOT NULL);"

        "INSERT INTO " WRD_AUTH_TABLE " (username, password, user_role, data)"
        "SELECT '" WRD_AUTH_DEFAULT_USER "', '" WRD_AUTH_DEFAULT_PASS "', '" WRD_AUTH_DEFAULT_ROLE "', '' "
        "WHERE NOT EXISTS (SELECT 1 FROM " WRD_AUTH_TABLE ");"

        "PRAGMA journal_mode = WAL;"
        "PRAGMA synchronous = NORMAL;"
        "PRAGMA busy_timeout = 5000;";
    // TODO: https://sqlite.org/lang_analyze.html#pragopt
    rc = sqlite3_exec(config->db, sql, 0, 0, &error_msg);

    if(rc != SQLITE_OK) {
        wrd_log_error(config, "SQL error: %s\n", error_msg);

        return WRD_DB_OPEN_ERROR;
    }

    WRD_DB_CALL(config->insert_stmt, sqlite3_prepare_v2(config->db, "INSERT INTO " WRD_ENTRIES_TABLE "(data) VALUES(json(?1));", -1, &config->insert_stmt, NULL));
    WRD_DB_CALL(config->insert_dashboard_stmt, sqlite3_prepare_v2(config->db, "INSERT INTO " WRD_DASHBOARDS_TABLE "(data) VALUES(json(?1));", -1, &config->insert_dashboard_stmt, NULL));
    WRD_DB_CALL(config->add_index_stmt, sqlite3_prepare_v2(config->db, "ALTER TABLE " WRD_ENTRIES_TABLE
        " add column \"?1\" as (json_extract(value, ?1));"
        "create index ?1 on " WRD_ENTRIES_TABLE "(?1);", -1, &config->add_index_stmt, NULL));
    WRD_DB_CALL(config->parse_json_stmt, sqlite3_prepare_v2(config->db, "SELECT * FROM json_each(?1) WHERE type='true';", -1, &config->parse_json_stmt, NULL));

    // return wrd_load_columns(config);
    return wrd_db_query_init(config);
}

wrd_code wrd_db_query_init(warudo *config) {
    CHECK_CONFIG

    int rc = sqlite3_open(":memory:", &config->query_db);
    char *error_msg = 0;

    if(rc != SQLITE_OK) {
        wrd_log_error(config, "Can't open query database: %s\n", sqlite3_errmsg(config->query_db));

        return WRD_DB_OPEN_ERROR;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS http_query(name TEXT PRIMARY KEY, value TEXT)";

    rc = sqlite3_exec(config->query_db, sql, 0, 0, &error_msg);

    if(rc != SQLITE_OK) {
        wrd_log_error(config, "SQL error: %s\n", error_msg);

        return WRD_DB_OPEN_ERROR;
    }

    rc = sqlite3_prepare_v2(config->query_db, "INSERT INTO http_query(name, value) VALUES(?1, ?2);", -1, &config->insert_query_stmt, NULL);

    if(rc != SQLITE_OK) {
        wrd_log_error(config, "SQL error: %s\n", sqlite3_errmsg(config->query_db));

        return WRD_DB_OPEN_ERROR;
    }

    return WRD_OK;
}

wrd_code wrd_db_close(warudo *config) {
    CHECK_CONFIG

    for(unsigned int i = 0; i < config->columns_count; ++i) {
        free(config->columns[i].name);
        free(config->columns[i].type);

        config->columns[i].name = NULL;
        config->columns[i].type = NULL;
    }

    config->columns_count = 0;

    // Finalize database queries
    sqlite3_finalize(config->insert_stmt);
    sqlite3_finalize(config->insert_dashboard_stmt);
    sqlite3_finalize(config->add_index_stmt);
    sqlite3_finalize(config->parse_json_stmt);

    // Close database.
    if(sqlite3_close(config->db) != SQLITE_OK) {
        return WRD_DB_CLOSE_ERROR;
    }

    // Finalize query database queries
    sqlite3_finalize(config->insert_query_stmt);

    // Close query database.
    if(sqlite3_close(config->query_db) != SQLITE_OK) {
        return WRD_DB_CLOSE_ERROR;
    }

    if(sqlite3_shutdown() != SQLITE_OK) {
        return WRD_DB_CLOSE_ERROR;
    }

    return WRD_OK;
}

wrd_code wrd_db_add_header(warudo *config, const char *name, const char *value) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 0;
    int must_output_error = 1;
    const char *query = NULL;
    int rc;
    sqlite3_stmt *stmt = config->insert_query_stmt;
    sqlite3_reset(stmt);

    WRD_DB_CALL(stmt, sqlite3_bind_text(stmt, 1, name, strlen(name), SQLITE_STATIC));
    WRD_DB_CALL(stmt, sqlite3_bind_text(stmt, 2, value, strlen(value), SQLITE_STATIC));
    WRD_DB_RET_CALL(stmt, sqlite3_step(stmt), SQLITE_DONE);

    return WRD_OK;
}

/*wrd_code wrd_load_columns(warudo *config) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 1;
    int must_output_error = 1;
    int rc;
    const char *query = "PRAGMA table_info('" WRD_ENTRIES_TABLE "');";
    sqlite3_stmt *stmt;

    WRD_DB_CALL(stmt, sqlite3_prepare_v2(config->db, query, -1, &stmt, NULL));

    wrd_log_info(config, "Column names for table '%s':\n", WRD_ENTRIES_TABLE);

    while(sqlite3_step(stmt) == SQLITE_ROW) {
        const char *name = (const char*)sqlite3_column_text(stmt, 1);
        const char *type = (const char*)sqlite3_column_text(stmt, 2);

        config->columns[config->columns_count].name = strdup((const char*)name);
        config->columns[config->columns_count].type = strdup((const char*)type);
        config->columns_count++;
    }

    for(unsigned int i = 0; i < config->columns_count; ++i) {
        wrd_log_info(config, "%s: %s\n", config->columns[i].name, config->columns[i].type);
    }

    sqlite3_finalize(stmt);

    return WRD_OK;
}*/

unsigned long long wrd_last_insert_rowid(warudo *config) {
    if(!config) {
        return 0;
    }

    return sqlite3_last_insert_rowid(config->db);
}

wrd_code wrd_parse_json(warudo *config) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 0;
    int must_output_error = 1;
    int rc;
    const char *query = NULL;
    sqlite3_stmt *stmt = config->parse_json_stmt;
    sqlite3_reset(stmt);

    long int length = wrd_content_length(config);

    if(length <= 0) {
        return WRD_EMPTY_CONTENT_ERROR;
    }

    char *json = wrd_read_content(length, config);

    if(json == NULL) {
        return WRD_READ_ERROR;
    }

    WRD_DB_CALL(stmt, sqlite3_bind_text(stmt, 1, json, length, SQLITE_STATIC));

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *key_name = (const char*)sqlite3_column_text(stmt, 0);

        if(strcmp(key_name, "key") == 0) {
            const char *value = (const char*)sqlite3_column_text(stmt, 2);

            if(strcmp(key_name, "string") == 0) {
                wrd_http_puts(config, value);
            }

            break;
        }
    }

    wrd_http_created(config, wrd_last_insert_rowid(config));
    free(json);

    return WRD_OK;
}

wrd_code wrd_add_index(const char *filename, warudo *config) {
    CHECK_CONFIG

    (void)filename;
    int must_free = 0;
    int must_finalize = 0;
    int must_output_error = 1;
    int rc;
    const char *query = NULL;
    sqlite3_stmt *stmt = config->add_index_stmt;
    sqlite3_reset(stmt);

    long int length = wrd_content_length(config);

    if(length <= 0) {
        return WRD_EMPTY_CONTENT_ERROR;
    }

    char *json = wrd_read_content(length, config);

    if(json == NULL) {
        return WRD_READ_ERROR;
    }

    WRD_DB_CALL(stmt, sqlite3_bind_text(stmt, 1, json, length, SQLITE_STATIC));
    WRD_DB_RET_CALL(stmt, sqlite3_step(stmt), SQLITE_DONE);

    wrd_http_created(config, wrd_last_insert_rowid(config));
    free(json);

    return WRD_OK;
}

wrd_code wrd_add_entry(int entry_type, warudo *config) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 0;
    int must_output_error = 1;
    int rc;
    const char *query = NULL;
    sqlite3_stmt *stmt = entry_type == WRD_ENTRY_TYPE_DATA ? config->insert_stmt : config->insert_dashboard_stmt;
    sqlite3_reset(stmt);

    long int length = wrd_content_length(config);

    if(length <= 0) {
        return WRD_EMPTY_CONTENT_ERROR;
    }

    char *json = wrd_read_content(length, config);

    if(json == NULL) {
        return WRD_READ_ERROR;
    }

    WRD_DB_CALL(stmt, sqlite3_bind_text(stmt, 1, json, length, SQLITE_STATIC));
    WRD_DB_RET_CALL(stmt, sqlite3_step(stmt), SQLITE_DONE);

    wrd_http_created(config, wrd_last_insert_rowid(config));
    free(json);

    return WRD_OK;
}

wrd_code wrd_formdata_callback(const char *input, long int length, warudo *config) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 0;
    int must_output_error = 0;
    int rc;
    const char *query = NULL;
    sqlite3_stmt *stmt = config->insert_stmt;
    sqlite3_reset(stmt);

    if(input == NULL) {
        return WRD_READ_ERROR;
    }

    WRD_DB_CALL(stmt, sqlite3_bind_text(stmt, 1, input, length, SQLITE_STATIC));
    WRD_DB_RET_CALL(stmt, sqlite3_step(stmt), SQLITE_DONE);

    return WRD_OK;
}

wrd_code wrd_add_entries(int entry_type, warudo *config) {
    CHECK_CONFIG

    (void)entry_type;
    int count = 0;
    long int length = wrd_content_length(config);

    if(length <= 0) {
        return WRD_EMPTY_CONTENT_ERROR;
    }

    char *input = wrd_read_content(length, config);
    char **value = NULL;

    if(wrd_http_get_header(config, "content-length", value) != WRD_OK) {
        wrd_http_bad_request(config, "Failed to get content length.");
    }

    const char *boundary = wrd_get_formdata_boundary(*value);
    count = wrd_parse_formdata(input, length, boundary, &wrd_formdata_callback, config);

    if(count <= 0) {
        wrd_http_bad_request(config, "No entries created.");
    } else {
        // wrd_http_ok(config);
        // wrd_http_puts(input, config);
        wrd_http_multi_created(config, count);
    }

    if(input) {
        free(input);
    }

    return count ? WRD_OK : WRD_EMPTY_CONTENT_ERROR;
}

wrd_code wrd_get_entries(int entry_type, warudo *config) {
    CHECK_CONFIG

    int must_free = 1;
    int must_finalize = 1;
    int must_output_error = 1;
    int rc;
    char *query;
    sqlite3_stmt *stmt;
    int count = 0;
    int limit_index = 0;
    int has_search = !config->query.id && config->query.key != NULL && config->query.value != NULL;
    const char *table_name = entry_type == WRD_ENTRY_TYPE_DATA ? "entries" : "dashboards";
    const char *order_by = config->query.orderby ? config->query.orderby : "id";
    const char *sort = config->query.sort ? config->query.sort : "ASC";

    /*
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
        query = sqlite3_mprintf("SELECT id, created, modified, JSON(data) FROM %q WHERE CAST(data ->> ?1 AS TEXT) LIKE ?2 ORDER BY %q %q LIMIT ?3 OFFSET ?4;", table_name, order_by, sort);
        limit_index = 3;
    } else if(config->query.id) {
        query = sqlite3_mprintf("SELECT id, created, modified, JSON(data) FROM %q WHERE id = ?1;", table_name);
    } else {
        query = sqlite3_mprintf("SELECT id, created, modified, JSON(data) FROM %q ORDER BY %q %q LIMIT ?1 OFFSET ?2;", table_name, order_by, sort);
        limit_index = 1;
    }

    WRD_DB_CALL(stmt, sqlite3_prepare_v2(config->db, query, -1, &stmt, NULL));

    if(has_search) {
        char *query_key = wrd_url_decode(config->query.key);
        char *query_value = wrd_url_decode(config->query.value);

        WRD_DB_CALL(stmt, sqlite3_bind_text(stmt, 1, config->query.key, strlen(config->query.key), SQLITE_STATIC));
        WRD_DB_CALL(stmt, sqlite3_bind_text(stmt, 2, config->query.value, strlen(config->query.value), SQLITE_STATIC));

        free(query_key);
        free(query_value);
    } else if(config->query.id) {
        WRD_DB_CALL(stmt, sqlite3_bind_int64(stmt, 1, config->query.id));
    }

    if(limit_index) {
        WRD_DB_CALL(stmt, sqlite3_bind_int(stmt, limit_index, config->query.limit));
        WRD_DB_CALL(stmt, sqlite3_bind_int64(stmt, limit_index + 1, config->query.offset));
    }

    wrd_http_ok(config);
    wrd_http_puts(config, "[");

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Retrieve the result
        sqlite3_int64 id = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 created = sqlite3_column_int64(stmt, 1);
        sqlite3_int64 modified = sqlite3_column_int64(stmt, 2);
        const char *data = (const char*)sqlite3_column_text(stmt, 3);

        if(count != 0) {
            wrd_http_puts(config, ",");
        }

        wrd_http_printf(config, "{\"id\":%lld,\"created\":%lld,\"modified\":%lld,\"data\":%s}",
            id, created, modified, data ? data : "{}");
        ++count;
    }

    wrd_http_puts(config, "]");
    sqlite3_finalize(stmt);

    if(must_free) {
        sqlite3_free(query);
    }

    return WRD_OK;
}

wrd_code wrd_get_keys(warudo *config) {
    CHECK_CONFIG

    int must_free = 0;
    int must_finalize = 1;
    int must_output_error = 1;
    int rc;
    char *query = "SELECT key, COUNT(*) AS occurrence_count FROM " WRD_ENTRIES_TABLE ", json_tree(data) GROUP BY key;";
    int count = 0;
    sqlite3_stmt *stmt;

    WRD_DB_CALL(stmt, sqlite3_prepare_v2(config->db, query, -1, &stmt, NULL));

    wrd_http_ok(config);
    wrd_http_puts(config, "[");

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Retrieve the result
        const char *key_name = (const char*)sqlite3_column_text(stmt, 0);
        sqlite3_int64 key_count = sqlite3_column_int64(stmt, 1);

        if(key_name == NULL) {
            continue;
        }

        if(count != 0) {
            wrd_http_puts(config, ",");
        }

        wrd_http_printf(config, "{\"name\":\"%s\",\"value\":%lld}", key_name, key_count);
        ++count;
    }

    wrd_http_puts(config, "]");
    sqlite3_finalize(stmt);

    return WRD_OK;
}

/*wrd_code wrd_add_index(const name, warudo *config) {
    char *query = "ALTER TABLE " WRD_ENTRIES_TABLE " ADD COLUMN ?1 TEXT;"
        "AS JSON_EXTRACT(data, '$.%q') AS %q;";
}*/
