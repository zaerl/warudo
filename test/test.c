/**
 * The warudo library tests
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

static unsigned int tests_valid = 0;
static unsigned int tests_total = 0;

void warudo_assert(int test, const char *message, int wait) {
    ++tests_total;
    char enter = 0;

    if(test) {
        printf("Test: %s \x1B[32mOK\x1B[0m\n", message);
        ++tests_valid;
    } else {
        printf("\x1B[31mTest: %s FAIL\x1B[0m\n", message);

        if(wait) {
            printf("Press any key to continue... ");

            while(enter != '\r' && enter != '\n' && enter != 27) {
                enter = getchar();

                if(enter == 27) {
                    exit(EXIT_SUCCESS);
                }
            }
        }
    }
}

int main(int argc, const char *argv[]) {
    printf("\x1b[Warudo %s test\x1B[0m\n", WARUDO_VERSION);
    (void)argc;
    (void)argv;

    RUN_TEST(db)
    RUN_TEST(init)
    RUN_TEST(warudo)

    int valid = tests_valid == tests_total;

    /* Green if valid and red if not */
    const char *color_code = valid ? "\x1B[32m" : "\x1B[31m";

    printf("\nTests valid/run: %s%d/%d\n\x1B[0m", color_code, tests_valid, tests_total);

    return valid ? 0 : -1;
}
