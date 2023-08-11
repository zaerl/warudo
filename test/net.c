#include "../src/net.h"
#include "test.h"

void test_net(void) {
    INIT_TEST

    ASSERT_NULL("NULL", warudo_url_decode, NULL)
    ASSERT_FSTRING("valid simple string", "valid", warudo_url_decode, "valid")
    ASSERT_FSTRING("valid full URL", "https://example.com/?ex=ex#ex=ex+ex", warudo_url_decode, "https%3A%2F%2Fexample.com%2F%3Fex%3Dex%23ex%3Dex%2Bex")

    ASSERT_FSTRING("valid hello world", " hello world", warudo_url_decode, "%20hello%20world")
    ASSERT_FSTRING("valid my/path/to/file", "my/path/to/file", warudo_url_decode, "my%2Fpath%2Fto%2Ffile")
    ASSERT_FSTRING("valid query=parameter&value=123", "query=parameter&value=123", warudo_url_decode, "query%3Dparameter%26value%3D123")
    ASSERT_FSTRING("valid special & characters", "special & characters", warudo_url_decode, "special%20%26%20characters")
    ASSERT_FSTRING("valid latin", "éçö", warudo_url_decode, "%C3%A9%C3%A7%C3%B6")
    ASSERT_FSTRING("valid %% + /=", "% + /=", warudo_url_decode, "%25%20%2B%20%2F%3D")
    ASSERT_FSTRING("valid omega +u0020", "Ω", warudo_url_decode, "%CE%A9")
    ASSERT_FSTRING("valid various", "€ % @", warudo_url_decode, "%E2%82%AC%20%25%20%40")
    ASSERT_FSTRING("valid umlaut = % @", "ö = % @", warudo_url_decode, "%C3%B6%20%3D%20%25%20%40")
    ASSERT_FSTRING("valid / ? # [ ]", " / ? # [ ]", warudo_url_decode, "%20%2F%20%3F%20%23%20%5B%20%5D")

    ASSERT_NULL("NULL", warudo_escape_html, NULL)
    ASSERT_FSTRING("empty", "", warudo_escape_html, "")
    ASSERT_FSTRING("a-z", "a-z", warudo_escape_html, "a-z")

    ASSERT_NULL("too short #1", warudo_get_formdata_boundary, "")
    ASSERT_NULL("too short #2", warudo_get_formdata_boundary, "\r")
    ASSERT_NULL("too short #3", warudo_get_formdata_boundary, "\r\n")
    ASSERT_NULL("too short #4", warudo_get_formdata_boundary, "multipart/form-data; boundary=")
    ASSERT_NULL("invalid", warudo_get_formdata_boundary, "application/json; charset=utf-8")
    ASSERT_NULL("no \\n", warudo_get_formdata_boundary, "multipart/form-data; boundary=\n")
    ASSERT_NULL("no $%&/", warudo_get_formdata_boundary, "multipart/form-data; boundary=$%&/")
    ASSERT_STRING("valid", "a", warudo_get_formdata_boundary, "multipart/form-data; boundary=a")
    ASSERT_STRING("valid", "az-_", warudo_get_formdata_boundary, "multipart/form-data; boundary=az-_")

    ASSERT("NULL", 0, is_valid_boundary, NULL)
    ASSERT("NULL", 0, is_valid_boundary, "")
    ASSERT("no \\n", 0, is_valid_boundary, "\n")
    ASSERT("no $%&/", 0, is_valid_boundary, "$%&/")
    ASSERT("valid az-_", 1, is_valid_boundary, "az-_")
}
