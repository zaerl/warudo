#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "zaerl.h"

int zaerl_load_columns(zaerl* config);

int zaerl_db_init(const char *filename, zaerl* config) {
    // Load database
    char* error_msg = 0;
    int rc = sqlite3_open(filename, &config->db);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(config->db));

        return 1;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS entries(id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT NOT NULL);"
        "PRAGMA journal_mode = WAL;"
        "PRAGMA synchronous = NORMAL;"
        "PRAGMA busy_timeout = 5000;";
    rc = sqlite3_exec(config->db, sql, 0, 0, &error_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_msg);

        return 1;
    }

    return zaerl_load_columns(config);
}

int zaerl_load_columns(zaerl* config) {
    const char* table_info_sql = "PRAGMA table_info('entries');";
    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(config->db, table_info_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(config->db));
        sqlite3_finalize(stmt);

        return 1;
    }

    fprintf(stderr, "Column names for table '%s':\n", "entries");

    while(sqlite3_step(stmt) == SQLITE_ROW) {
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        const char* type = (const char*)sqlite3_column_text(stmt, 2);

        config->columns[config->columns_count].name = strdup((const char*)name);
        config->columns[config->columns_count].type = strdup((const char*)type);
        config->columns_count++;
    }

    for(unsigned int i = 0; i < config->columns_count; ++i) {
        printf("%s: %s\n", config->columns[i].name, config->columns[i].type);
    }

    sqlite3_finalize(stmt);

    return 0;
}

int zaerl_add_entry(zaerl *config, const char* json) {
    const char* sql = "INSERT INTO entries(data) VALUES(?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(config->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(config->db));
        sqlite3_finalize(stmt);

        return 1;
    }

    sqlite3_bind_text(stmt, 1, json, strlen(json), SQLITE_STATIC);

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(config->db));
        sqlite3_finalize(stmt);

        return 1;
    }

    return 0;
}

int zaerl_db_close(zaerl *config) {
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
