/**
 * The warudo tests
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <getopt.h>
#include <pthread.h>
#include <time.h>

#include "../src/version.h"
#include "test.h"

int show_version(void) {
    printf("%s v%s\n", WRD_NAME, WRD_VERSION);

    return 0;
}

void show_help(const char *executable, struct option options[], const char *descriptions[], const char *error) {
    FILE *stream = error ? stderr : stdout;

    fprintf(stream, "%s%swarudo - Warudo test client [v%s]\n\nUsage: %s [OPTIONS]\n",
        error ? error : "",
        error ? "\n\n" : "",
        WRD_VERSION, executable);
    fprintf(stream, "Options:\n");

    for(unsigned long i = 0; i < 3; ++i) {
        fprintf(stream, "  -%c%s, --%s%s\n\t%s\n",
            options[i].val,
            options[i].has_arg == no_argument ? "" : " ARG",
            options[i].name,
            options[i].has_arg == no_argument ? "" : "=ARG",
            descriptions[i]);
    }
}

int main(int argc, char * const argv[]) {
    struct timespec start, end;
    double elapsed;
    unsigned int verbosity = 2;
    int option;
    int option_index = 0;
    char *filter = NULL;

    struct option long_options[] = {
        { "filter", required_argument, NULL, 'f' },
        { "help", no_argument, NULL, 'h' },
        { "verbose", no_argument, NULL, 'v' },
        { "version", no_argument, NULL, 'V' },
        { NULL, 0, NULL, 0 }
    };
    const char *descriptions[] = {
        "Filter tests by name in the form name1,name2,...",
        "Show this help message",
        "Verbose output",
        "Print version"
    };

    clock_gettime(CLOCK_MONOTONIC, &start);
    att_set_verbose(verbosity);
    att_set_verbose(1);

    while((option = getopt_long(argc, argv, "f:hvV", long_options, &option_index)) != -1) {
        switch(option) {
            case 'f':
                filter = strdup(optarg);
                break;
            case 'h':
                show_help(argv[0], long_options, descriptions, NULL);
                return 0;
            case 'v':
                att_set_verbose(2);
                break;
            case 'V':
                return show_version();
            case '?':
                // getopt_long already printed an error message
                break;
            default:
                abort();
        }
    }

    #define RUN_TEST(NAME) \
        if(!filter || strcasestr(filter, #NAME)) { \
            printf("\nTest: \x1b[1;32m%s\x1b[0m\n", #NAME); \
            pthread_t thread_##NAME; \
            pthread_create(&thread_##NAME, NULL, test_##NAME, NULL); \
            pthread_join(thread_##NAME, NULL); \
        }

    // Start tests declarations.
    RUN_TEST(auth)
    RUN_TEST(code)
    RUN_TEST(conf)
    RUN_TEST(data)
    RUN_TEST(db)
    RUN_TEST(env)
    RUN_TEST(fs)
    RUN_TEST(hpack)
    RUN_TEST(http)
    RUN_TEST(log)
    RUN_TEST(net)
    RUN_TEST(output)
    RUN_TEST(query)
    RUN_TEST(queue)
    RUN_TEST(route_app)
    RUN_TEST(route_home)
    RUN_TEST(server)
    RUN_TEST(timing)
    RUN_TEST(tls)
    RUN_TEST(version)

    unsigned int tests_valid = att_get_valid_tests();
    unsigned int tests_total = att_get_total_tests();
    int valid = tests_valid == tests_total;

    /* Green if valid and red if not */
    const char *color_code = valid ? "\x1B[32m" : "\x1B[31m";

    printf("\n%sTests valid/run: %s%d/%d\n\x1B[0m", verbosity == 1 ? "\n" : "", color_code, tests_valid, tests_total);

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Execution time: %.4f seconds\n", elapsed);

    if(filter) {
        free(filter);
    }

    return valid ? 0 : -1;
}
