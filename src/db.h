#ifndef WARUDO_DB_H
#define WARUDO_DB_H

#include "warudo.h"

warudo_code warudo_db_init(const char *filename, warudo* config);
warudo_code warudo_parse_json(warudo* config);
warudo_code warudo_add_index(const char *filename, warudo* config);
warudo_code warudo_db_close(warudo *config);
warudo_code warudo_load_columns(warudo* config);
unsigned long long warudo_last_insert_rowid(warudo *config);

warudo_code warudo_add_entry(int entry_type, warudo *config);
warudo_code warudo_formdata_callback(const char* input, long int length, warudo* config);
warudo_code warudo_add_entries(int entry_type, warudo *config);
warudo_code warudo_get_entries(int entry_type, warudo *config);
warudo_code warudo_get_keys(warudo *config);


#endif /* WARUDO_DB_H */
