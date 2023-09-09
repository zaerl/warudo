#include "../src/data.h"
#include "test.h"

long test_file_size(FILE *file) {
    fseek(file, 0, SEEK_END);

    return ftell(file);
}

void *test_data(void *arg) {
    MOCK_FILE(temp_file)

    ASSERT("NULL", 0, wrd_debug_dump, NULL, NULL, NULL, 0)
    ASSERT("NULL ptr", 0, wrd_debug_dump, "test", NULL, NULL, 0)

    // test, 0000000000 bytes (0x00000000)
    ASSERT("0 written", 0, wrd_debug_dump, "test", temp_file, (unsigned char*)"test", 0)
    ASSERT("36 file size", 36, test_file_size, temp_file)

    MOCK_FILE(temp_file_2)

    // test, 0000000016 bytes (0x00000010)
    // 0000: 74 65 73 74 74 65 73 74 74 65 73 74 74 65 73 74 testtesttesttest
    ASSERT("16 written", 16, wrd_debug_dump, "test", temp_file_2, "testtesttesttest", 16)
    ASSERT("107 file size", 36 + 6 + 16 * 3 + 16 + 1, test_file_size, temp_file_2)

    MOCK_FILE(temp_file_3)

    // test, 0000000001 bytes (0x00000001)
    // 0000: 74                                              t
    ASSERT("1 size, 1 written", 1, wrd_debug_dump, "test", temp_file_3, "testtesttesttest", 1)
    ASSERT("1 size, 92 file size", 36 + 6 + 16 * 3 + 1 + 1, test_file_size, temp_file_3)

    return NULL;
}
