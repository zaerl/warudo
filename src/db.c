#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "warudo.h"

int warudo_load_columns(warudo* config);

int warudo_db_init(const char *filename, warudo* config) {
    // Load database
    char* error_msg = 0;
    int rc = sqlite3_open(filename, &config->db);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(config->db));

        return 1;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS " WARUDO_ENTRIES_TABLE "(id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT NOT NULL);"
        "CREATE TABLE IF NOT EXISTS " WARUDO_VIEWS_TABLE "("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "created INTEGER DEFAULT (UNIXEPOCH()), "
        "modified INTEGER DEFAULT (UNIXEPOCH()), "
        "data TEXT NOT NULL);"
        "PRAGMA journal_mode = WAL;"
        "PRAGMA synchronous = NORMAL;"
        "PRAGMA busy_timeout = 5000;";
    rc = sqlite3_exec(config->db, sql, 0, 0, &error_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_msg);

        return 1;
    }

    // return warudo_load_columns(config);
    return 0;
}

int warudo_load_columns(warudo* config) {
    const char* table_info_sql = "PRAGMA table_info('" WARUDO_ENTRIES_TABLE "');";
    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(config->db, table_info_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(config->db));
        sqlite3_finalize(stmt);

        return 1;
    }

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

int warudo_add_entry(int entry_type, const char* json, warudo *config) {
    sqlite3_stmt* stmt;
    const char* sql = entry_type == WARUDO_ENTRY_TYPE_DATA ? "INSERT INTO entries(data) VALUES(json(?));" :
        "INSERT INTO views(data) VALUES(json(?));";

    if(json == NULL) {
        return 1;
    }

    if (sqlite3_prepare_v2(config->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(config->db));
        sqlite3_finalize(stmt);

        return 1;
    }

    if(sqlite3_bind_text(stmt, 1, json, strlen(json), SQLITE_STATIC) != SQLITE_OK) {
        fprintf(stderr, "Failed to bind text: %s\n", sqlite3_errmsg(config->db));
        sqlite3_finalize(stmt);

        return 1;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(config->db));
        sqlite3_finalize(stmt);

        return 1;
    }

    return 0;
}

int warudo_get_entries(int entry_type, warudo *config) {
    const char *query;
    sqlite3_stmt *stmt;
    int rc;
    int count = 0;
    int limit_index = 1;
    int has_search = config->query_key != NULL && config->query_value != NULL;

    if(has_search) {
        query = entry_type == WARUDO_ENTRY_TYPE_DATA ? "SELECT data FROM entries WHERE data ->> ? = ? LIMIT ?;" :
            "SELECT id, created_at, data FROM views WHERE data ->> ? = ? LIMIT ?;";
        limit_index = 3;
    } else {
        query = entry_type == WARUDO_ENTRY_TYPE_DATA ? "SELECT data FROM entries LIMIT ?;" :
            "SELECT id, created, modified, data FROM views LIMIT ?;";
    }

    if (sqlite3_prepare_v2(config->db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(config->db));
        sqlite3_finalize(stmt);

        return 1;
    }

    if(has_search) {
        if(sqlite3_bind_text(stmt, 1, config->query_key, strlen(config->query_key), SQLITE_STATIC) != SQLITE_OK) {
            fprintf(stderr, "Failed to bind text: %s\n", sqlite3_errmsg(config->db));
            sqlite3_finalize(stmt);

            return 1;
        }

        if(sqlite3_bind_text(stmt, 2, config->query_value, strlen(config->query_value), SQLITE_STATIC) != SQLITE_OK) {
            fprintf(stderr, "Failed to bind text: %s\n", sqlite3_errmsg(config->db));
            sqlite3_finalize(stmt);

            return 1;
        }
    }

    if(sqlite3_bind_int(stmt, limit_index, config->query_limit) != SQLITE_OK) {
        fprintf(stderr, "Failed to bind limit: %s\n", sqlite3_errmsg(config->db));
        sqlite3_finalize(stmt);

        return 1;
    }

    FCGX_PutS("[", config->request.out);

    if(entry_type == WARUDO_ENTRY_TYPE_DATA) {
        while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            // Retrieve the result
            const char *data = (const char*)sqlite3_column_text(stmt, 0);

            if(count != 0) {
                FCGX_PutS(",", config->request.out);
            }

            FCGX_PutS(data, config->request.out);
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

    return 0;
}

int warudo_get_keys(warudo *config) {
    const char *query = "SELECT key, COUNT(*) AS occurrence_count FROM " WARUDO_ENTRIES_TABLE ", json_tree(data) GROUP BY key;";
    int rc;
    int count = 0;
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(config->db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(config->db));
        sqlite3_finalize(stmt);

        return 1;
    }

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

    return 0;
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
