#ifndef ZAERL_DB_H
#define ZAERL_DB_H

#include "zaerl.h"

int zaerl_db_init(const char *filename, zaerl* config);
int zaerl_db_close(zaerl *config);

int zaerl_add_entry(zaerl *db);

#endif /* ZAERL_DB_H */
