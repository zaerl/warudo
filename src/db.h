#ifndef WRD_DB_H
#define WRD_DB_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

wrd_code wrd_db_init(const char *filename, warudo *config);
wrd_code wrd_parse_json(warudo *config);
wrd_code wrd_add_index(const char *filename, warudo *config);
wrd_code wrd_db_close(warudo *config);
// wrd_code wrd_load_columns(warudo *config);
unsigned long long wrd_last_insert_rowid(warudo *config);

wrd_code wrd_add_entry(int entry_type, warudo *config);
wrd_code wrd_formdata_callback(const char *input, long int length, warudo *config);
wrd_code wrd_add_entries(int entry_type, warudo *config);
wrd_code wrd_get_entries(int entry_type, warudo *config);
wrd_code wrd_get_keys(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WRD_DB_H
