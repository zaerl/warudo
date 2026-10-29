#include "../test.h"
#include "../../src/app/app.h"

void *test_db(void *arg) {
    ASSERT_CODE(wrd_db_close(NULL), WRD_ERROR, "wrd_db_close NULL")
    ASSERT_CODE(wrd_db_init(NULL, NULL), WRD_ERROR, "wrd_db_init NULL")
    ASSERT_CODE(wrd_parse_json(NULL), WRD_ERROR, "wrd_parse_json NULL")
    ASSERT_CODE(wrd_add_index("", NULL), WRD_ERROR, "wrd_add_index NULL")
    ASSERT_CODE(wrd_db_close(NULL), WRD_ERROR, "wrd_db_close NULL")
    // ASSERT_CODE(wrd_load_columns(NULL), WRD_ERROR, "NULL")
    ASSERT_CODE(wrd_last_insert_rowid(NULL), 0, "wrd_last_insert_rowid NULL")

    ASSERT_CODE(wrd_add_entry(0, NULL), WRD_ERROR, "wrd_add_entr* NULL")
    ASSERT_CODE(wrd_add_entries(0, NULL), WRD_ERROR, "wrd_add_entr* NULL")
    ASSERT_CODE(wrd_get_entries(0, NULL), WRD_ERROR, "wrd_add_entr* NULL")
    ASSERT_CODE(wrd_get_keys(NULL), WRD_ERROR, "wrd_get_keys NULL")

    ATT_ASSERT(wrd_db_order_by(NULL), "id", "wrd_db_order_by default");
    ATT_ASSERT(wrd_db_order_by("created"), "created", "wrd_db_order_by created");
    ATT_ASSERT(wrd_db_order_by("modified"), "modified", "wrd_db_order_by modified");
    ATT_ASSERT(wrd_db_order_by("data"), NULL, "wrd_db_order_by not whitelisted");
    ATT_ASSERT(wrd_db_order_by("id;--"), NULL, "wrd_db_order_by injection");
    ATT_ASSERT(wrd_db_order_by("(SELECT password FROM http_auth)"), NULL, "wrd_db_order_by subquery");

    ATT_ASSERT(wrd_db_sort(NULL), "ASC", "wrd_db_sort default");
    ATT_ASSERT(wrd_db_sort("asc"), "ASC", "wrd_db_sort asc");
    ATT_ASSERT(wrd_db_sort("DESC"), "DESC", "wrd_db_sort DESC");
    ATT_ASSERT(wrd_db_sort("desc"), "DESC", "wrd_db_sort desc");
    ATT_ASSERT(wrd_db_sort("DESC, id"), NULL, "wrd_db_sort injection");

    return NULL;
}
