#include "../src/query.h"
#include "test.h"

static int formdata_result = WRD_OK;

int internal_parse_formdata_callback(const char *input, long int length, warudo *config) {
    return formdata_result;
}

void test_url_decode() {
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

void test_form() {

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

void *test_query(void *arg) {
    MOCK_CONFIG

    ASSERT_CODE(wrd_parse_query_string(NULL, NULL), WRD_ERROR, "wrd_parse_query_string NULL")

    test_url_decode();
    test_form();

    return NULL;
}
