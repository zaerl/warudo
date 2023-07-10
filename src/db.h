#ifndef WARUDO_DB_H
#define WARUDO_DB_H

#include "warudo.h"

int warudo_db_init(const char *filename, warudo* config);
int warudo_db_close(warudo *config);

int warudo_add_entry(int entry_type, const char* json, warudo *config);
int warudo_get_entries(int entry_type, warudo *config);

int warudo_get_keys(warudo *config);

#endif /* WARUDO_DB_H */
