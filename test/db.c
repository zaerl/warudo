#include "../src/db.h"
#include "test.h"

void test_db(void) {
    INIT_TEST

    ASSERT_ERROR(warudo_db_close(NULL), "warudo_db_close NULL")

    ASSERT_ERROR(warudo_db_init(NULL, NULL), "warudo_db_init NULL")
    ASSERT_ERROR(warudo_parse_json(NULL), "warudo_parse_json NULL")
    ASSERT_ERROR(warudo_add_index(NULL, NULL), "warudo_add_index NULL")
    ASSERT_ERROR(warudo_db_close(NULL), "warudo_db_close NULL")
    ASSERT_ERROR(warudo_load_columns(NULL), "warudo_load_columns NULL")
    ASSERT_ERROR(warudo_last_insert_rowid(NULL), "warudo_last_insert_rowid NULL")

    ASSERT_ERROR(warudo_add_entry(0, NULL), "warudo_add_entry NULL")
    ASSERT_ERROR(warudo_add_entries(0, NULL), "warudo_add_entries NULL")
    ASSERT_ERROR(warudo_get_entries(0, NULL), "warudo_get_entries NULL")
    ASSERT_ERROR(warudo_get_keys(NULL), "warudo_get_keys NULL")
}
