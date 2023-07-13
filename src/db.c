#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "warudo.h"
#include "config.h"
#include "net.h"

#define WARUDO_DB_RET_CALL(CALL, RET) \
    if(CALL != RET) { \
    fprintf(stderr, "Failed to execute db query: %s\n", sqlite3_errmsg(config->db)); \
    sqlite3_finalize(stmt); \
    return 1; } \

#define WARUDO_DB_CALL(CALL) WARUDO_DB_RET_CALL(CALL, SQLITE_OK)

int warudo_load_columns(warudo* config);

int warudo_db_init(const char *filename, warudo* config) {
    // Load database
    char* error_msg = 0;
    int rc = sqlite3_open(filename, &config->db);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(config->db));

        return 1;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS " WARUDO_ENTRIES_TABLE "("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT NOT NULL, "
        "created INTEGER DEFAULT (UNIXEPOCH()));"
        "CREATE TABLE IF NOT EXISTS " WARUDO_VIEWS_TABLE "("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created INTEGER DEFAULT (UNIXEPOCH()), "
        "modified INTEGER DEFAULT (UNIXEPOCH()), "
        "data TEXT NOT NULL);"
        "PRAGMA journal_mode = WAL;"
        "PRAGMA synchronous = NORMAL;"
        "PRAGMA busy_timeout = 5000;";
    rc = sqlite3_exec(config->db, sql, 0, 0, &error_msg);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_msg);

        return 1;
    }

    // return warudo_load_columns(config);
    return 0;
}

int warudo_load_columns(warudo* config) {
    const char* table_info_sql = "PRAGMA table_info('" WARUDO_ENTRIES_TABLE "');";
    sqlite3_stmt* stmt;

    WARUDO_DB_CALL(sqlite3_prepare_v2(config->db, table_info_sql, -1, &stmt, NULL));

    fprintf(stderr, "Column names for table '%s':\n", WARUDO_ENTRIES_TABLE);

    while(sqlite3_step(stmt) == SQLITE_ROW) {
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        const char* type = (const char*)sqlite3_column_text(stmt, 2);

        config->columns[config->columns_count].name = strdup((const char*)name);
        config->columns[config->columns_count].type = strdup((const char*)type);
        config->columns_count++;
    }

    for(unsigned int i = 0; i < config->columns_count; ++i) {
        fprintf(stderr, "%s: %s\n", config->columns[i].name, config->columns[i].type);
    }

    sqlite3_finalize(stmt);

    return 0;
}

int warudo_add_entry(int entry_type, warudo *config) {
    sqlite3_stmt* stmt;
    const char* sql = entry_type == WARUDO_ENTRY_TYPE_DATA ? "INSERT INTO entries(data) VALUES(json(?));" :
        "INSERT INTO views(data) VALUES(json(?));";

    char* json = warudo_read_content(config, 0);

    if(json == NULL) {
        return 1;
    }

    WARUDO_DB_CALL(sqlite3_prepare_v2(config->db, sql, -1, &stmt, NULL));
    WARUDO_DB_CALL(sqlite3_bind_text(stmt, 1, json, strlen(json), SQLITE_STATIC));
    WARUDO_DB_RET_CALL(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);
    free(json);

    return 0;
}

int warudo_add_entries(int entry_type, warudo *config) {
    sqlite3_stmt* stmt;
    const char* sql = entry_type == WARUDO_ENTRY_TYPE_DATA ? "INSERT INTO entries(data) VALUES(json(?));" :
        "INSERT INTO views(data) VALUES(json(?));";

    char* json = warudo_read_content(config, 0);

    if(json == NULL) {
        return 1;
    }

    WARUDO_DB_CALL(sqlite3_prepare_v2(config->db, sql, -1, &stmt, NULL));
    WARUDO_DB_CALL(sqlite3_bind_text(stmt, 1, json, strlen(json), SQLITE_STATIC));
    WARUDO_DB_RET_CALL(sqlite3_step(stmt), SQLITE_DONE);

    sqlite3_finalize(stmt);
    free(json);

    return 0;
}

int warudo_get_entries(int entry_type, warudo *config) {
    const char *query;
    sqlite3_stmt *stmt;
    int rc;
    int count = 0;
    int limit_index = 0;
    int has_search = !config->query_id && config->query_key != NULL && config->query_value != NULL;

    if(has_search) {
        query = entry_type == WARUDO_ENTRY_TYPE_DATA ? "SELECT id, created, data FROM entries WHERE data ->> ? = ? LIMIT ? OFFSET ?;" :
            "SELECT id, created, modified, data FROM views WHERE data ->> ? = ? LIMIT ? OFFSET ?;";
        limit_index = 3;
    } else if(config->query_id) {
        query = entry_type == WARUDO_ENTRY_TYPE_DATA ? "SELECT id, created, data FROM entries WHERE id = ?;" :
            "SELECT id, created, modified, data FROM views WHERE id= ?;";
    } else {
        query = entry_type == WARUDO_ENTRY_TYPE_DATA ? "SELECT id, created, data FROM entries LIMIT ? OFFSET ?;" :
            "SELECT id, created, modified, data FROM views LIMIT ? OFFSET ?;";
        limit_index = 1;
    }

    WARUDO_DB_CALL(sqlite3_prepare_v2(config->db, query, -1, &stmt, NULL));

    if(has_search) {
        WARUDO_DB_CALL(sqlite3_bind_text(stmt, 1, config->query_key, strlen(config->query_key), SQLITE_STATIC));
        WARUDO_DB_CALL(sqlite3_bind_text(stmt, 2, config->query_value, strlen(config->query_value), SQLITE_STATIC));
    } else if(config->query_id) {
        WARUDO_DB_CALL(sqlite3_bind_int64(stmt, 1, config->query_id));
    }

    if(limit_index) {
        WARUDO_DB_CALL(sqlite3_bind_int(stmt, limit_index, config->query_limit));
        WARUDO_DB_CALL(sqlite3_bind_int64(stmt, limit_index + 1, config->query_offset));
    }

    FCGX_PutS("[", config->request.out);

    if(entry_type == WARUDO_ENTRY_TYPE_DATA) {
        while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            // Retrieve the result
            sqlite3_int64 id = sqlite3_column_int64(stmt, 0);
            sqlite3_int64 created = sqlite3_column_int64(stmt, 1);
            const char *data = (const char*)sqlite3_column_text(stmt, 2);

            if(count != 0) {
                FCGX_PutS(",", config->request.out);
            }

            FCGX_FPrintF(config->request.out, "{\"id\":%lld,\"created\":%lld,\"data\":%s}", id, created, data ? data : "{}");
            ++count;
        }
    } else if(entry_type == WARUDO_ENTRY_TYPE_VIEW) {
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
    }

    FCGX_PutS("]", config->request.out);
    sqlite3_finalize(stmt);

    return 0;
}

int warudo_get_keys(warudo *config) {
    const char *query = "SELECT key, COUNT(*) AS occurrence_count FROM " WARUDO_ENTRIES_TABLE ", json_tree(data) GROUP BY key;";
    int rc;
    int count = 0;
    sqlite3_stmt *stmt;

    WARUDO_DB_CALL(sqlite3_prepare_v2(config->db, query, -1, &stmt, NULL));

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

        FCGX_FPrintF(config->request.out, "{\"name\":\"%s\",\"count\":%lld}", key_name, key_count);
        ++count;
    }

    FCGX_PutS("]", config->request.out);
    sqlite3_finalize(stmt);

    return 0;
}

unsigned long long int warudo_last_insert_rowid(warudo *config) {
    return sqlite3_last_insert_rowid(config->db);
}

int warudo_db_close(warudo *config) {
    if(!config) {
        return 0;
    }

    for(unsigned int i = 0; i < config->columns_count; ++i) {
        free(config->columns[i].name);
        free(config->columns[i].type);

        config->columns[i].name = NULL;
        config->columns[i].type = NULL;
    }

    config->columns_count = 0;

    return sqlite3_close(config->db) == SQLITE_OK ? 0 : 1;
}
