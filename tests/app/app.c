#include "../test.h"
#include "../../src/app/app.h"

void test_parse_query_string(void) {
    wrd_query query = {0};

    // NULL and empty return OK with defaults.
    ASSERT_CODE(wrd_app_parse_query_string(&query, NULL), WRD_OK, "NULL query string")
    ATT_ASSERT(query.limit, WRD_DEFAULT_QUERY_LIMIT, "Default limit after NULL");

    char qs1[] = "";
    ASSERT_CODE(wrd_app_parse_query_string(&query, qs1), WRD_OK, "Empty query string")
    ATT_ASSERT(query.limit, WRD_DEFAULT_QUERY_LIMIT, "Default limit after empty");

    // Parse limit.
    char qs2[] = "limit=5";
    ASSERT_CODE(wrd_app_parse_query_string(&query, qs2), WRD_OK, "Parse limit")
    ATT_ASSERT(query.limit, 5, "Limit is 5");

    // Parse offset.
    char qs3[] = "offset=20";
    ASSERT_CODE(wrd_app_parse_query_string(&query, qs3), WRD_OK, "Parse offset")
    ATT_ASSERT(query.offset, 20, "Offset is 20");

    // Parse id.
    char qs4[] = "id=42";
    ASSERT_CODE(wrd_app_parse_query_string(&query, qs4), WRD_OK, "Parse id")
    ATT_ASSERT(query.id, 42, "Id is 42");

    // Parse multi.
    char qs5[] = "multi=1";
    ASSERT_CODE(wrd_app_parse_query_string(&query, qs5), WRD_OK, "Parse multi")
    ATT_ASSERT(query.multi, 1, "Multi is 1");

    // Parse multiple parameters.
    char qs6[] = "limit=3&offset=10&id=7";
    ASSERT_CODE(wrd_app_parse_query_string(&query, qs6), WRD_OK, "Parse multiple params")
    ATT_ASSERT(query.limit, 3, "Limit is 3");
    ATT_ASSERT(query.offset, 10, "Offset is 10");
    ATT_ASSERT(query.id, 7, "Id is 7");

    // Parse key and value.
    char qs7[] = "key=name&value=test";
    ASSERT_CODE(wrd_app_parse_query_string(&query, qs7), WRD_OK, "Parse key and value")
    ATT_ASSERT(strcmp(query.key, "name"), 0, "Key is name");
    ATT_ASSERT(strcmp(query.value, "test"), 0, "Value is test");
    free((void*)query.key);
    free((void*)query.value);

    // Parse orderby and sort.
    char qs8[] = "orderby=created&sort=DESC";
    ASSERT_CODE(wrd_app_parse_query_string(&query, qs8), WRD_OK, "Parse orderby and sort")
    ATT_ASSERT(strcmp(query.orderby, "created"), 0, "Orderby is created");
    ATT_ASSERT(strcmp(query.sort, "DESC"), 0, "Sort is DESC");
    free((void*)query.orderby);
    free((void*)query.sort);

    // Parameter without value.
    char qs9[] = "limit=";
    ASSERT_CODE(wrd_app_parse_query_string(&query, qs9), WRD_OK, "Parse empty value")
    ATT_ASSERT(query.limit, 0, "Limit with empty value is 0");

    // Defaults are reset on each call.
    char qs10[] = "id=99";
    ASSERT_CODE(wrd_app_parse_query_string(&query, qs10), WRD_OK, "Defaults reset")
    ATT_ASSERT(query.limit, WRD_DEFAULT_QUERY_LIMIT, "Limit reset to default");
    ATT_ASSERT(query.id, 99, "Id is 99");
}

void *test_app(void *arg) {
    ASSERT_CODE(wrd_app_init(NULL), WRD_ERROR, "wrd_app_init NULL")
    ASSERT_CODE(wrd_app_close(NULL), WRD_ERROR, "wrd_app_close NULL")
    ASSERT_CODE(wrd_app_parse_query_string(NULL, NULL), WRD_ERROR, "wrd_app_parse_query_string NULL")
    ASSERT_CODE(wrd_app_free_query(NULL), WRD_ERROR, "wrd_app_free_query NULL")
    ASSERT_CODE(wrd_route_home(NULL), WRD_ERROR, "Home NULL")
    ASSERT_CODE(wrd_route_entries(NULL), WRD_ERROR, "Entries NULL")
    ASSERT_CODE(wrd_route_dashboards(NULL), WRD_ERROR, "Dashboards NULL")
    ASSERT_CODE(wrd_route_app_keys(NULL), WRD_ERROR, "App keys NULL")
    ASSERT_CODE(wrd_http_created(NULL, 1), WRD_ERROR, "wrd_http_created NULL")
    ASSERT_CODE(wrd_http_multi_created(NULL, 1), WRD_ERROR, "wrd_http_multi_created NULL")

    // A loaded config without app data is rejected by the app routes.
    MOCK_CONFIG
    ASSERT_CODE(wrd_route_entries(&config), WRD_ERROR, "Entries without app data")
    ASSERT_CODE(wrd_route_app_keys(&config), WRD_ERROR, "App keys without app data")
    ASSERT_CODE(wrd_app_close(&config), WRD_ERROR, "wrd_app_close without app data")
    wrd_config_close(&config);

    test_parse_query_string();

    return NULL;
}
