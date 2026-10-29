#include "test.h"

static int formdata_result = WRD_OK;

int internal_parse_formdata_callback(const char *input, long int length, warudo *config) {
    return formdata_result;
}

void test_url_decode(void) {
    ATT_ASSERT(wrd_url_decode(NULL), NULL, "NULL")
    ATT_ASSERT(wrd_url_decode("valid"), "valid", "valid simple string")
    ATT_ASSERT(wrd_url_decode("https%3A%2F%2Fexample.com%2F%3Fex%3Dex%23ex%3Dex%2Bex"), "https://example.com/?ex=ex#ex=ex+ex", "valid full URL")

    ATT_ASSERT(wrd_url_decode("%20hello%20world"), " hello world", "valid hello world")
    ATT_ASSERT(wrd_url_decode("my%2Fpath%2Fto%2Ffile"), "my/path/to/file", "valid my/path/to/file")
    ATT_ASSERT(wrd_url_decode("query%3Dparameter%26value%3D123"), "query=parameter&value=123", "valid query=parameter&value=123")
    ATT_ASSERT(wrd_url_decode("special%20%26%20characters"), "special & characters", "valid special & characters")
    ATT_ASSERT(wrd_url_decode("%C3%A9%C3%A7%C3%B6"), "éçö", "valid latin")
    ATT_ASSERT(wrd_url_decode("%25%20%2B%20%2F%3D"), "% + /=", "valid %% + /=")
    ATT_ASSERT(wrd_url_decode("%CE%A9"), "Ω", "valid omega +u0020")
    ATT_ASSERT(wrd_url_decode("%E2%82%AC%20%25%20%40"), "€ % @", "valid various")
    ATT_ASSERT(wrd_url_decode("%C3%B6%20%3D%20%25%20%40"), "ö = % @", "valid umlaut = % @")
    ATT_ASSERT(wrd_url_decode("%20%2F%20%3F%20%23%20%5B%20%5D"), " / ? # [ ]", "valid / ? # [ ]")
}

void test_form(void) {
    MOCK_CONFIG

    ATT_ASSERT((char*)wrd_get_formdata_boundary(""), NULL, "too short #1")
    ATT_ASSERT((char*)wrd_get_formdata_boundary("\r"), NULL, "too short #2")
    ATT_ASSERT((char*)wrd_get_formdata_boundary("\r\n"), NULL, "too short #3")
    ATT_ASSERT((char*)wrd_get_formdata_boundary("multipart/form-data; boundary="), NULL, "too short #4")
    ATT_ASSERT((char*)wrd_get_formdata_boundary("application/json; charset=utf-8"), NULL, "invalid")
    ATT_ASSERT((char*)wrd_get_formdata_boundary("multipart/form-data; boundary=\n"), NULL, "no \\n")
    ATT_ASSERT((char*)wrd_get_formdata_boundary("multipart/form-data; boundary=$%&/"), NULL, "no $%&/")
    ATT_ASSERT((char*)wrd_get_formdata_boundary("multipart/form-data; boundary=a"), "a", "valid a")
    ATT_ASSERT((char*)wrd_get_formdata_boundary("multipart/form-data; boundary=az-_"), "az-_", "valid az-_")

    ATT_ASSERT(wrd_is_valid_boundary(NULL), 0, "NULL")
    ATT_ASSERT(wrd_is_valid_boundary(""), 0, "NULL")
    ATT_ASSERT(wrd_is_valid_boundary("\n"), 0, "no \\n")
    ATT_ASSERT(wrd_is_valid_boundary("$%&/"), 0, "no $%&/")
    ATT_ASSERT(wrd_is_valid_boundary("az-_"), 1, "valid az-_")

    ASSERT_CODE(wrd_parse_formdata(NULL, 0, NULL, NULL, &config), WRD_PARSER_EMPTY, "NULL all")
    ASSERT_CODE(wrd_parse_formdata("test", 4, NULL, NULL, &config), WRD_PARSER_EMPTY, "NULL boundary")
    ASSERT_CODE(wrd_parse_formdata("test", 4, "", NULL, &config), WRD_PARSER_EMPTY, "empty boundary")
    ASSERT_CODE(wrd_parse_formdata("test", 4, "test", NULL, &config), WRD_PARSER_EMPTY, "NULL callback")
    ASSERT_CODE(wrd_parse_formdata("test", 4, "test", internal_parse_formdata_callback, NULL), WRD_PARSER_EMPTY, "NULL config")
    ASSERT_CODE(wrd_parse_formdata("test", 4, "test", internal_parse_formdata_callback, &config), WRD_PARSER_VOID, "too short #1")

    const char *test = "0123456789"
        "0123456789"
        "0123456789"
        "0123456789"
        "0123456789"
        "0123456789"
        "0123456789"
        "0123456789";

    ASSERT_CODE(wrd_parse_formdata(test, 54, "b", internal_parse_formdata_callback, &config), WRD_PARSER_VOID, "too short #2")
    ASSERT_CODE(wrd_parse_formdata(test, 70, "b", internal_parse_formdata_callback, &config), WRD_PARSER_NO_BOUNDARY, "too short #3")

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\ntesttesttesttest";
    ASSERT_CODE(wrd_parse_formdata(test, strlen(test), "", internal_parse_formdata_callback, &config), WRD_PARSER_EMPTY_BOUNDARY, "empty boundary")

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n--b--\r\n";
    ASSERT_CODE(wrd_parse_formdata(test, strlen(test), "b", internal_parse_formdata_callback, &config), WRD_PARSER_NO_BOUNDARY, "missing content")

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n\r\n--b--\r\n";
    ASSERT_CODE(wrd_parse_formdata(test, strlen(test), "b", internal_parse_formdata_callback, &config), WRD_PARSER_MISSING_CONTENT, "missing JSON")

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{\"test\":\"test\"}\"}";
    ASSERT_CODE(wrd_parse_formdata(test, strlen(test), "b", internal_parse_formdata_callback, &config), WRD_PARSER_NO_BOUNDARY, "minimal missing end")

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\ntesttesttesttest";
    ASSERT_CODE(wrd_parse_formdata(test, strlen(test), "b", internal_parse_formdata_callback, &config), WRD_PARSER_NO_BOUNDARY, "missing end")

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{}\r\n--b--\r\n";
    ASSERT_CODE(wrd_parse_formdata(test, strlen(test), "b", internal_parse_formdata_callback, &config), 1, "minimal valid")

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{}\r\n--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{}\r\n--b--\r\n";
    ASSERT_CODE(wrd_parse_formdata(test, strlen(test), "b", internal_parse_formdata_callback, &config), 2, "valid #2")

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{}\r\n--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{}\r\n--b--\r\n";
    ASSERT_CODE(wrd_parse_formdata(test, strlen(test), "b", internal_parse_formdata_callback, &config), 2, "valid #2")
}

void test_parse_query_string(void) {
    MOCK_CONFIG

    // NULL and empty return OK with defaults.
    ASSERT_CODE(wrd_parse_query_string(&config, NULL), WRD_OK, "NULL query string")
    ATT_ASSERT(config.query.limit, WRD_DEFAULT_QUERY_LIMIT, "Default limit after NULL")

    char qs1[] = "";
    ASSERT_CODE(wrd_parse_query_string(&config, qs1), WRD_OK, "Empty query string")
    ATT_ASSERT(config.query.limit, WRD_DEFAULT_QUERY_LIMIT, "Default limit after empty")

    // Parse limit.
    char qs2[] = "limit=5";
    ASSERT_CODE(wrd_parse_query_string(&config, qs2), WRD_OK, "Parse limit")
    ATT_ASSERT(config.query.limit, 5, "Limit is 5")

    // Parse offset.
    char qs3[] = "offset=20";
    ASSERT_CODE(wrd_parse_query_string(&config, qs3), WRD_OK, "Parse offset")
    ATT_ASSERT(config.query.offset, 20, "Offset is 20")

    // Parse id.
    char qs4[] = "id=42";
    ASSERT_CODE(wrd_parse_query_string(&config, qs4), WRD_OK, "Parse id")
    ATT_ASSERT(config.query.id, 42, "Id is 42")

    // Parse multi.
    char qs5[] = "multi=1";
    ASSERT_CODE(wrd_parse_query_string(&config, qs5), WRD_OK, "Parse multi")
    ATT_ASSERT(config.query.multi, 1, "Multi is 1")

    // Parse multiple parameters.
    char qs6[] = "limit=3&offset=10&id=7";
    ASSERT_CODE(wrd_parse_query_string(&config, qs6), WRD_OK, "Parse multiple params")
    ATT_ASSERT(config.query.limit, 3, "Limit is 3")
    ATT_ASSERT(config.query.offset, 10, "Offset is 10")
    ATT_ASSERT(config.query.id, 7, "Id is 7")

    // Parse key and value.
    char qs7[] = "key=name&value=test";
    ASSERT_CODE(wrd_parse_query_string(&config, qs7), WRD_OK, "Parse key and value")
    ATT_ASSERT(strcmp(config.query.key, "name"), 0, "Key is name")
    ATT_ASSERT(strcmp(config.query.value, "test"), 0, "Value is test")
    free((void*)config.query.key);
    free((void*)config.query.value);

    // Parse orderby and sort.
    char qs8[] = "orderby=created&sort=DESC";
    ASSERT_CODE(wrd_parse_query_string(&config, qs8), WRD_OK, "Parse orderby and sort")
    ATT_ASSERT(strcmp(config.query.orderby, "created"), 0, "Orderby is created")
    ATT_ASSERT(strcmp(config.query.sort, "DESC"), 0, "Sort is DESC")
    free((void*)config.query.orderby);
    free((void*)config.query.sort);

    // Parameter without value.
    char qs9[] = "limit=";
    ASSERT_CODE(wrd_parse_query_string(&config, qs9), WRD_OK, "Parse empty value")
    ATT_ASSERT(config.query.limit, 0, "Limit with empty value is 0")

    // Defaults are reset on each call.
    char qs10[] = "id=99";
    ASSERT_CODE(wrd_parse_query_string(&config, qs10), WRD_OK, "Defaults reset")
    ATT_ASSERT(config.query.limit, WRD_DEFAULT_QUERY_LIMIT, "Limit reset to default")
    ATT_ASSERT(config.query.id, 99, "Id is 99")
}

void *test_query(void *arg) {
    ASSERT_CODE(wrd_parse_query_string(NULL, NULL), WRD_ERROR, "wrd_parse_query_string NULL")

    test_url_decode();
    test_form();
    test_parse_query_string();

    return NULL;
}
