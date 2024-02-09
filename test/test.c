/**
 * The warudo tests
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <pthread.h>
#include <time.h>

#include "test.h"
#include "attractor.h"
#include "../src/code.h"

#define DECLARE_TEST(NAME) void *test_##NAME(void *arg);

DECLARE_TEST(auth)
DECLARE_TEST(code)
DECLARE_TEST(data)
DECLARE_TEST(db)
DECLARE_TEST(init)
DECLARE_TEST(log)
DECLARE_TEST(net)
DECLARE_TEST(output)
DECLARE_TEST(route_app)
DECLARE_TEST(route_home)
DECLARE_TEST(time)
DECLARE_TEST(warudo)

int main(int argc, const char *argv[]) {
    struct timespec start, end;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);
    att_set_verbose(2);

    #define RUN_TEST(NAME) \
        pthread_t thread_##NAME; \
        pthread_create(&thread_##NAME, NULL, test_##NAME, NULL); \
        pthread_join(thread_##NAME, NULL);

    RUN_TEST(auth)
    RUN_TEST(code)
    RUN_TEST(data)
    RUN_TEST(db)
    RUN_TEST(init)
    RUN_TEST(log)
    RUN_TEST(net)
    RUN_TEST(output)
    RUN_TEST(route_app)
    RUN_TEST(route_home)
    RUN_TEST(time)
    RUN_TEST(warudo)

    unsigned int tests_valid = att_get_valid_tests();
    unsigned int tests_total = att_get_total_tests();
    int valid = tests_valid == tests_total;

    /* Green if valid and red if not */
    const char *color_code = valid ? "\x1B[32m" : "\x1B[31m";

    printf("Tests valid/run: %s%d/%d\n\x1B[0m", color_code, tests_valid, tests_total);

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Execution time: %.4f seconds\n", elapsed);

    return valid ? 0 : -1;
}

void wrd_assert_code(int result, int expected, const char *description) {
    int test = result == expected;

    att_set_show_error(0);
    ATT_ASSERT(result, expected, description);
    att_set_show_error(1);

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
