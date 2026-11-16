#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "test.h"

// Generate a temporary file with the given content.
char *wrd_mock_file(const char *content) {
    char file_template[] = "wrd_mock_file_XXXXXX";
    int fd = mkstemp(file_template);

    if(fd == -1) {
        ATT_ASSERT(0, 1, "mkstemp() failed");

        return NULL;
    }

    if(content) {
        if(write(fd, content, strlen(content)) == -1) {
            ATT_ASSERT(0, 1, "fputs() failed");

            return NULL;
        }
    }

    close(fd);

    return strndup(file_template, 20);
}

void wrd_assert_code(int result, int expected, const char *description) {
    int test = result == expected;

    att_set_show_error(0);
    ATT_ASSERT(result, expected, description);
    att_set_show_error(1);

    if(!test) {
        char str1[20];
        char str2[20];

        const char *result_desc = wrd_error_description(static_cast<wrd_code>(result), 0);
        const char *expected_desc = wrd_error_description(static_cast<wrd_code>(expected), 0);

        if(result_desc == NULL) {
            snprintf(str1, sizeof(str1), "%d", result);
        }

        if(expected_desc == NULL) {
            snprintf(str2, sizeof(str2), "%d", expected);
        }

        printf("Expected \x1B[32m%s\x1B[0m, got \x1B[31m%s\x1B[0m\n\n",
            expected_desc ? expected_desc : str2, result_desc ? result_desc : str1);
    }
}
