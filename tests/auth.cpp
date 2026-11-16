/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include "test.h"

ATT_TEST(auth) {
    ATT_ASSERT(WRD_OWNED(wrd_base64_encode(NULL)), nullptr, "NULL");
    ATT_ASSERT(WRD_OWNED(wrd_base64_encode("")), "", "empty string");

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
        ATT_ASSERT(WRD_OWNED(wrd_base64_encode(input[i])), (char*)output[i], input[i]);
    }

    ATT_ASSERT(WRD_OWNED(wrd_base64_decode(NULL)), nullptr, "NULL");
    ATT_ASSERT(WRD_OWNED(wrd_base64_decode("")), "", "empty string");
    ATT_ASSERT(WRD_OWNED(wrd_base64_decode("A")), nullptr, "single character");
    ATT_ASSERT(WRD_OWNED(wrd_base64_decode("AB")), nullptr, "two characters");
    ATT_ASSERT(WRD_OWNED(wrd_base64_decode("ABC")), nullptr, "three characters");

    for(int i = 0; i < 6; ++i) {
        ATT_ASSERT(WRD_OWNED(wrd_base64_decode(output[i])), (char*)input[i], output[i]);
    }

    ATT_ASSERT(wrd_base64_decode("$%&/()=?"), nullptr, "string with invalid chars");
    ATT_ASSERT(WRD_OWNED(wrd_base64_decode("QUJDR==A")), "ABCD", "string with out-of-place pad");
}
