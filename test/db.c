#include "../src/db.h"
#include "test.h"

void test_db(void) {
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_db_close, NULL)

    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_db_init, NULL, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_parse_json, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_add_index, "", NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_db_close, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_load_columns, NULL)
    ASSERT("NULL", 0, warudo_last_insert_rowid, NULL)

    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_add_entry, 0, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_add_entries, 0, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_get_entries, 0, NULL)
    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_get_keys, NULL)
}
