#include <stdio.h>
#include <stdlib.h>
#include "db.h"

int zaerl_init(const char *filename, zaerl **db) {
    zaerl *pdb;
    char* error_msg = 0;
    int rc;

    pdb = malloc(sizeof(zaerl));
    rc = sqlite3_open(filename, &pdb->db);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(pdb->db));
        zaerl_close(pdb);

        return 1;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS entries(id INTEGER PRIMARY KEY AUTOINCREMENT);";
    rc = sqlite3_exec(pdb->db, sql, 0, 0, &error_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", error_msg);
        sqlite3_free(error_msg);
        zaerl_close(pdb);

        return 1;
    }

    // sqlite3_close(pdb->db);

    *db = pdb;

    return 0;
}

int zaerl_close(zaerl *db) {
    if(!db) {
        return 0;
    }

    sqlite3_close(db->db);
    free(db);

    return 0;
}
