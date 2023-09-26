#include "../src/auth.h"
#include "test.h"

void *test_auth(void *arg) {
    ASSERT("NULL", NULL, wrd_base64_encode, NULL)
    ASSERT("empty string", "", wrd_base64_encode, "")

    const char *input[] = {
        "A",
        "AB",
        "ABC",
        "ABCD",
        "ABCDE",
        "ABCDEF"
    };

    const char *output[] = {
        "QQ==",
        "QUI=",
        "QUJD",
        "QUJDRA==",
        "QUJDREU=",
        "QUJDREVG"
    };

    for(int i = 0; i < 6; ++i) {
        ASSERT("base64 encode", output[i], wrd_base64_encode, input[i]);
    }

    ASSERT("NULL", NULL, wrd_base64_decode, NULL)
    ASSERT("empty string", "", wrd_base64_decode, "")
    ASSERT("single character", NULL, wrd_base64_decode, "A")
    ASSERT("two characters", NULL, wrd_base64_decode, "AB")
    ASSERT("three characters", NULL, wrd_base64_decode, "ABC")

    for(int i = 0; i < 6; ++i) {
        ASSERT("base64 decode", input[i], wrd_base64_decode, output[i]);
    }

    ASSERT("string with invalid chars", NULL, wrd_base64_decode, "$%&/()=?")
    ASSERT("string with out-of-place pad", "ABCD", wrd_base64_decode, "QUJDR==A")

    return NULL;
}
