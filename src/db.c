#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "db.h"

int zaerl_db_init(const char *filename, zaerl* config) {
    // Load database
    char* error_msg = 0;
    int rc = sqlite3_open(filename, &config->db);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(config->db));

        return 1;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS entries(id INTEGER PRIMARY KEY AUTOINCREMENT);";
    rc = sqlite3_exec(config->db, sql, 0, 0, &error_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_msg);

        return 1;
    }

    return 0;
}
