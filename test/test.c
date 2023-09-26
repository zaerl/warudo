/**
 * The warudo tests
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <pthread.h>
#include <time.h>

#include "test.h"
#include "../src/code.h"

#define DECLARE_TEST(NAME) void *test_##NAME(void *arg);

DECLARE_TEST(auth)
DECLARE_TEST(code)
DECLARE_TEST(data)
DECLARE_TEST(db)
DECLARE_TEST(fcgi)
DECLARE_TEST(init)
DECLARE_TEST(log)
DECLARE_TEST(net)
DECLARE_TEST(route_app)
DECLARE_TEST(route_home)
DECLARE_TEST(warudo)

static unsigned int tests_valid = 0;
static unsigned int tests_total = 0;

void wrd_assert(const char *type, int test, const char *func_name, const char *description);

void wrd_assert_code(int result, int expected, const char *func_name, const char *description) {
    int test = result == expected;

    wrd_assert("int", test, func_name, description);

    if(!test) {
        char str1[20];
        char str2[20];

        char *result_desc = wrd_error_description(result, 0);
        char *expected_desc = wrd_error_description(expected, 0);

        if(result_desc == NULL) {
            sprintf(str1, "%d", result);
        }

        if(expected_desc == NULL) {
            sprintf(str2, "%d", expected);
        }

        printf("Expected \x1B[32m%s\x1B[0m, got \x1B[31m%s\x1B[0m\n\n",
            expected_desc ? expected_desc : str2, result_desc ? result_desc : str1);
    }
}

void wrd_assert_int(int result, int expected, const char *func_name, const char *description) {
    int test = result == expected;

    wrd_assert("int", test, func_name, description);

    if(!test) {
        printf("Expected \x1B[32m%d\x1B[0m, got \x1B[31m%d\x1B[0m\n\n", expected, result);
    }
}

void wrd_assert_string(char *result, char *expected, const char *func_name, const char *description) {
    wrd_assert_const_string(result, expected, func_name, description);

    if(result) {
        free(result);
    }
}

void wrd_assert_const_string(const char *result, char *expected, const char *func_name, const char *description) {
    int test = result && expected ? strcmp(result, expected) == 0 : 0;

    wrd_assert("string", test, func_name, description);

    if(!test) {
        printf("Expected \"\x1B[32m%s\x1B[0m\", got \"\x1B[31m%s\x1B[0m\"\n\n", expected ? expected : "NULL", result ? result : "NULL");
    }
}

void wrd_assert(const char *type, int test, const char *func_name, const char *description) {
    ++tests_total;

    const char *ok = "\x1B[32mOK\x1B[0m";
    const char *fail = "\x1B[31mFAIL\x1B[0m";
    int length = 80 - (strlen(type) + strlen(func_name) + strlen(description) + (test ? 2 : 4) + 5);

    if(length <= 0) {
        length = 2;
    }

    char spaces[length + 1];
    spaces[length] = '\0';

    for(int i = 0; i < length; i++) {
        spaces[i] = ' ';
    }

    printf("[%s] \x1b[34m%s\x1b[0m: %s%s%s\n", type, func_name, description, spaces, test ? ok : fail);

    if(test) {
        ++tests_valid;
    }
}

int main(int argc, const char *argv[]) {
    struct timespec start, end;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);

    #define RUN_TEST(NAME) \
        pthread_t thread_##NAME; \
        pthread_create(&thread_##NAME, NULL, test_##NAME, NULL); \
        pthread_join(thread_##NAME, NULL);

    RUN_TEST(auth)
    RUN_TEST(code)
    RUN_TEST(data)
    RUN_TEST(db)
    RUN_TEST(fcgi)
    RUN_TEST(init)
    RUN_TEST(log)
    RUN_TEST(net)
    RUN_TEST(route_app)
    RUN_TEST(route_home)
    RUN_TEST(warudo)

    int valid = tests_valid == tests_total;

    /* Green if valid and red if not */
    const char *color_code = valid ? "\x1B[32m" : "\x1B[31m";

    printf("\nTests valid/run: %s%d/%d\n\x1B[0m", color_code, tests_valid, tests_total);

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Execution time: %.4f seconds\n", elapsed);

    return valid ? 0 : -1;
}
