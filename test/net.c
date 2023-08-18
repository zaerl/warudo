#include "../src/net.h"
#include "test.h"

static int formdata_result = WARUDO_OK;

int internal_parse_formdata_callback(const char* input, long int length, warudo* config) {
    return formdata_result;
}

void* test_net(void* arg) {
    MOCK_CONFIG

    ASSERT("NULL", NULL, warudo_url_decode, NULL)
    ASSERT("valid simple string", "valid", warudo_url_decode, "valid")
    ASSERT("valid full URL", "https://example.com/?ex=ex#ex=ex+ex", warudo_url_decode, "https%3A%2F%2Fexample.com%2F%3Fex%3Dex%23ex%3Dex%2Bex")

    ASSERT("valid hello world", " hello world", warudo_url_decode, "%20hello%20world")
    ASSERT("valid my/path/to/file", "my/path/to/file", warudo_url_decode, "my%2Fpath%2Fto%2Ffile")
    ASSERT("valid query=parameter&value=123", "query=parameter&value=123", warudo_url_decode, "query%3Dparameter%26value%3D123")
    ASSERT("valid special & characters", "special & characters", warudo_url_decode, "special%20%26%20characters")
    ASSERT("valid latin", "éçö", warudo_url_decode, "%C3%A9%C3%A7%C3%B6")
    ASSERT("valid %% + /=", "% + /=", warudo_url_decode, "%25%20%2B%20%2F%3D")
    ASSERT("valid omega +u0020", "Ω", warudo_url_decode, "%CE%A9")
    ASSERT("valid various", "€ % @", warudo_url_decode, "%E2%82%AC%20%25%20%40")
    ASSERT("valid umlaut = % @", "ö = % @", warudo_url_decode, "%C3%B6%20%3D%20%25%20%40")
    ASSERT("valid / ? # [ ]", " / ? # [ ]", warudo_url_decode, "%20%2F%20%3F%20%23%20%5B%20%5D")

    ASSERT("NULL", NULL, warudo_escape_html, NULL)
    ASSERT("empty", "", warudo_escape_html, "")
    ASSERT("a-z", "a-z", warudo_escape_html, "a-z")

    ASSERT("too short #1", NULL, warudo_get_formdata_boundary, "")
    ASSERT("too short #2", NULL, warudo_get_formdata_boundary, "\r")
    ASSERT("too short #3", NULL, warudo_get_formdata_boundary, "\r\n")
    ASSERT("too short #4", NULL, warudo_get_formdata_boundary, "multipart/form-data; boundary=")
    ASSERT("invalid", NULL, warudo_get_formdata_boundary, "application/json; charset=utf-8")
    ASSERT("no \\n", NULL, warudo_get_formdata_boundary, "multipart/form-data; boundary=\n")
    ASSERT("no $%&/", NULL, warudo_get_formdata_boundary, "multipart/form-data; boundary=$%&/")
    ASSERT("valid", "a", warudo_get_formdata_boundary, "multipart/form-data; boundary=a")
    ASSERT("valid", "az-_", warudo_get_formdata_boundary, "multipart/form-data; boundary=az-_")

    ASSERT("NULL", 0, warudo_is_valid_boundary, NULL)
    ASSERT("NULL", 0, warudo_is_valid_boundary, "")
    ASSERT("no \\n", 0, warudo_is_valid_boundary, "\n")
    ASSERT("no $%&/", 0, warudo_is_valid_boundary, "$%&/")
    ASSERT("valid az-_", 1, warudo_is_valid_boundary, "az-_")

    ASSERT_CODE("NULL all", WARUDO_PARSER_EMPTY, warudo_parse_formdata, NULL, 0, NULL, NULL, &config)
    ASSERT_CODE("NULL boundary", WARUDO_PARSER_EMPTY, warudo_parse_formdata, "test", 4, NULL, NULL, &config)
    ASSERT_CODE("empty boundary", WARUDO_PARSER_EMPTY, warudo_parse_formdata, "test", 4, "", NULL, &config)
    ASSERT_CODE("NULL callback", WARUDO_PARSER_EMPTY, warudo_parse_formdata, "test", 4, "test", NULL, &config)
    ASSERT_CODE("NULL config", WARUDO_PARSER_EMPTY, warudo_parse_formdata, "test", 4, "test", internal_parse_formdata_callback, NULL)
    ASSERT_CODE("too short #1", WARUDO_PARSER_VOID, warudo_parse_formdata, "test", 4, "test", internal_parse_formdata_callback, &config)

    const char* test = "0123456789"
        "0123456789"
        "0123456789"
        "0123456789"
        "0123456789"
        "0123456789"
        "0123456789"
        "0123456789";

    ASSERT_CODE("too short #2", WARUDO_PARSER_VOID, warudo_parse_formdata, test, 54, "b", internal_parse_formdata_callback, &config)
    ASSERT_CODE("too short #3", WARUDO_PARSER_NO_BOUNDARY, warudo_parse_formdata, test, 70, "b", internal_parse_formdata_callback, &config)

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\ntesttesttesttest";
    ASSERT_CODE("empty boundary", WARUDO_PARSER_EMPTY_BOUNDARY, warudo_parse_formdata, test, strlen(test), "", internal_parse_formdata_callback, &config)

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n--b--\r\n";
    ASSERT_CODE("missing content", WARUDO_PARSER_NO_BOUNDARY, warudo_parse_formdata, test, strlen(test), "b", internal_parse_formdata_callback, &config)

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n\r\n--b--\r\n";
    ASSERT_CODE("missing JSON", WARUDO_PARSER_MISSING_CONTENT, warudo_parse_formdata, test, strlen(test), "b", internal_parse_formdata_callback, &config)

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{\"test\":\"test\"}\"}";
    ASSERT_CODE("minimal missing end", WARUDO_PARSER_NO_BOUNDARY, warudo_parse_formdata, test, strlen(test), "b", internal_parse_formdata_callback, &config)

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\ntesttesttesttest";
    ASSERT_CODE("missing end", WARUDO_PARSER_NO_BOUNDARY, warudo_parse_formdata, test, strlen(test), "b", internal_parse_formdata_callback, &config)

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{}\r\n--b--\r\n";
    ASSERT_CODE("minimal valid", 1, warudo_parse_formdata, test, strlen(test), "b", internal_parse_formdata_callback, &config)

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{}\r\n--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{}\r\n--b--\r\n";
    ASSERT_CODE("valid #2", 2, warudo_parse_formdata, test, strlen(test), "b", internal_parse_formdata_callback, &config)

    test = "--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{}\r\n--b\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n{}\r\n--b--\r\n";
    ASSERT_CODE("valid #2", 2, warudo_parse_formdata, test, strlen(test), "b", internal_parse_formdata_callback, &config)

    return NULL;
}
