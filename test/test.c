/**
 * The warudo tests
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include "test.h"
#include "../src/code.h"

static unsigned int tests_valid = 0;
static unsigned int tests_total = 0;

void warudo_assert(const char* type, int test, int wait, const char *func_name, const char *description);

void warudo_assert_code(int result, int expected, int wait, const char *func_name, const char *description) {
    int test = result == expected;

    warudo_assert("int", test, wait, func_name, description);

    if(!test) {
        printf("Expected \x1B[32m%s\x1B[0m, got \x1B[31m%s\x1B[0m\n\n",
            warudo_error_description(expected, 0), warudo_error_description(result, 0));
    }
}

void warudo_assert_int(int result, int expected, int wait, const char *func_name, const char *description) {
    int test = result == expected;

    warudo_assert("int", test, wait, func_name, description);

    if(!test) {
        printf("Expected \x1B[32m%d\x1B[0m, got \x1B[31m%d\x1B[0m\n\n", expected, result);
    }
}

void warudo_assert_string(char* result, char* expected, int wait, const char *func_name, const char *description) {
    int test = strcmp(result, expected) == 0;

    warudo_assert("string", test, wait, func_name, description);

    if(!test) {
        printf("Expected \"\x1B[32m%s\x1B[0m\", got \"\x1B[31m%d\x1B[0m\"\n\n", expected, result);
    }
}

void warudo_assert(const char* type, int test, int wait, const char *func_name, const char *description) {
    ++tests_total;
    char enter = 0;

    const char* ok = "\x1B[32mOK\x1B[0m";
    const char* fail = "\x1B[31mFAIL\x1B[0m";
    int length = 80 - (strlen(type) + strlen(func_name) + strlen(description) + (test ? 2 : 4) + 5);

    if(length <= 0) {
        length = 2;
    }

    char spaces[length + 1];
    spaces[length] = '\0';

    for(int i = 0; i < length; i++) {
        spaces[i] = ' ';
    }

    if(test) {
        printf("[%s] %s: %s%s%s\n", type, func_name, description, spaces, ok);
        ++tests_valid;
    } else {
        printf("[%s] %s: %s%s%s\n", type, func_name, description, spaces, fail);

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
    (void)argc;
    (void)argv;

    #define RUN_TEST(NAME) test_##NAME();

    RUN_TEST(app)
    RUN_TEST(code)
    RUN_TEST(db)
    RUN_TEST(home)
    RUN_TEST(init)
    RUN_TEST(log)
    RUN_TEST(net)
    RUN_TEST(warudo)

    int valid = tests_valid == tests_total;

    /* Green if valid and red if not */
    const char *color_code = valid ? "\x1B[32m" : "\x1B[31m";

    printf("\nTests valid/run: %s%d/%d\n\x1B[0m", color_code, tests_valid, tests_total);

    return valid ? 0 : -1;
}
