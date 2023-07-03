#ifndef ZAERL_DB_H
#define ZAERL_DB_H

#include "sqlite3.h"

struct zaerl {
    sqlite3 *db;
};

typedef struct zaerl zaerl;

int zaerl_init(const char *filename, zaerl **db);

int zaerl_close(zaerl *db);

int zaerl_add_entry(zaerl *db);

#endif /* ZAERL_DB_H */
