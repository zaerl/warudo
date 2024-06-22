/**
 * The warudo tests
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <pthread.h>
#include <time.h>

#include "test.h"

int main(int argc, const char *argv[]) {
    struct timespec start, end;
    double elapsed;
    unsigned int verbosity = 2;

    clock_gettime(CLOCK_MONOTONIC, &start);
    att_set_verbose(verbosity);

    #define RUN_TEST(NAME) \
        pthread_t thread_##NAME; \
        pthread_create(&thread_##NAME, NULL, test_##NAME, NULL); \
        pthread_join(thread_##NAME, NULL);

    // Start tests declarations.
    RUN_TEST(auth)
    RUN_TEST(code)
    RUN_TEST(conf)
    RUN_TEST(data)
    RUN_TEST(db)
    RUN_TEST(env)
    RUN_TEST(hpack)
    RUN_TEST(http)
    RUN_TEST(log)
    RUN_TEST(net)
    RUN_TEST(output)
    RUN_TEST(query)
    RUN_TEST(route_app)
    RUN_TEST(route_home)
    RUN_TEST(server)
    RUN_TEST(timing)
    RUN_TEST(version)

    unsigned int tests_valid = att_get_valid_tests();
    unsigned int tests_total = att_get_total_tests();
    int valid = tests_valid == tests_total;

    /* Green if valid and red if not */
    const char *color_code = valid ? "\x1B[32m" : "\x1B[31m";

    printf("%sTests valid/run: %s%d/%d\n\x1B[0m", verbosity == 1 ? "\n" : "", color_code, tests_valid, tests_total);

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Execution time: %.4f seconds\n", elapsed);

    return valid ? 0 : -1;
}
