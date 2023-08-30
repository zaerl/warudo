#include "../src/db.h"
#include "test.h"

void *test_db(void *arg) {
    ASSERT_CODE("NULL", WRD_ERROR, wrd_db_close, NULL)

    ASSERT_CODE("NULL", WRD_ERROR, wrd_db_init, NULL, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_parse_json, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_add_index, "", NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_db_close, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_load_columns, NULL)
    ASSERT("NULL", 0, wrd_last_insert_rowid, NULL)

    ASSERT_CODE("NULL", WRD_ERROR, wrd_add_entry, 0, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_add_entries, 0, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_get_entries, 0, NULL)
    ASSERT_CODE("NULL", WRD_ERROR, wrd_get_keys, NULL)

    return NULL;
}
