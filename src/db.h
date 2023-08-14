#ifndef WARUDO_DB_H
#define WARUDO_DB_H

#include "warudo.h"

int warudo_db_init(const char *filename, warudo* config);
int warudo_parse_json(warudo* config);
int warudo_add_index(const char *filename, warudo* config);
int warudo_db_close(warudo *config);
int warudo_load_columns(warudo* config);
unsigned long long int warudo_last_insert_rowid(warudo *config);

int warudo_add_entry(int entry_type, warudo *config);
int warudo_formdata_callback(const char* input, long int length, warudo* config);
int warudo_add_entries(int entry_type, warudo *config);
int warudo_get_entries(int entry_type, warudo *config);
int warudo_get_keys(warudo *config);


#endif /* WARUDO_DB_H */
