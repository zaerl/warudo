#include "../src/auth.h"
#include "test.h"

void *test_auth(void *arg) {
    ATT_ASSERT(wrd_base64_encode(NULL), NULL, "NULL")
    ATT_ASSERT(wrd_base64_encode(""), "", "empty string")

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
        ATT_ASSERT(wrd_base64_encode(input[i]), (char*)output[i], input[i]);
    }

    ATT_ASSERT(wrd_base64_decode(NULL), NULL, "NULL")
    ATT_ASSERT(wrd_base64_decode(""), "", "empty string")
    ATT_ASSERT(wrd_base64_decode("A"), NULL, "single character")
    ATT_ASSERT(wrd_base64_decode("AB"), NULL, "two characters")
    ATT_ASSERT(wrd_base64_decode("ABC"), NULL, "three characters")

    for(int i = 0; i < 6; ++i) {
        ATT_ASSERT(wrd_base64_decode(output[i]), (char*)input[i], output[i])
    }

    ATT_ASSERT(wrd_base64_decode("$%&/()=?"), NULL, "string with invalid chars")
    ATT_ASSERT(wrd_base64_decode("QUJDR==A"), "ABCD", "string with out-of-place pad")

    return NULL;
}
