#include "../src/net.h"
#include "test.h"

void test_net(void) {
    INIT_TEST

    ASSERT_NULL("NULL", warudo_url_decode, NULL)
    ASSERT_FSTRING("valid simple string", "valid", warudo_url_decode, "valid")
    ASSERT_FSTRING("valid full URL", "https://example.com/?ex=ex#ex=ex+ex", warudo_url_decode, "https%3A%2F%2Fexample.com%2F%3Fex%3Dex%23ex%3Dex%2Bex")

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
