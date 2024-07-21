#include "../cli/data.h"
#include "test.h"

long test_file_size(FILE *file) {
    fseek(file, 0, SEEK_END);

    return ftell(file);
}

void *test_data(void *arg) {
    WRD_TMP_FILE(temp_file)

    ATT_ASSERT(wrd_debug_dump(NULL, NULL, NULL, 0), 0, "wrd_debug_dump NULL")
    ATT_ASSERT(wrd_debug_dump("test", NULL, NULL, 0), 0, "wrd_debug_dump NULL ptr")

    // test, 0000000000 bytes (0x00000000)
    ATT_ASSERT(wrd_debug_dump("test", temp_file, (unsigned char*)"test", 0), 0, "0 written")
    ATT_ASSERT(test_file_size(temp_file), 36, "36 file size")

    WRD_TMP_FILE(temp_file_2)

    // test, 0000000016 bytes (0x00000010)
    // 0000: 74 65 73 74 74 65 73 74 74 65 73 74 74 65 73 74 testtesttesttest
    ATT_ASSERT(wrd_debug_dump("test", temp_file_2, (unsigned char*)"testtesttesttest", 16), 16, "16 written")
    ATT_ASSERT(test_file_size(temp_file_2), 36 + 6 + 16 * 3 + 16 + 1, "107 file size")

    WRD_TMP_FILE(temp_file_3)

    // test, 0000000001 bytes (0x00000001)
    // 0000: 74                                              t
    ATT_ASSERT(wrd_debug_dump("test", temp_file_3, (unsigned char*)"testtesttesttest", 1), 1, "1 size, 1 written")
    ATT_ASSERT(test_file_size(temp_file_3), 36 + 6 + 16 * 3 + 1 + 1, "1 size, 92 file size")

    return NULL;
}
