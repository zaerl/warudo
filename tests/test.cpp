/**
 * The warudo tests
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <getopt.h>
#include <time.h>

#include "test.h"

#define ATT_IMPLEMENTATION
#include "attractor.h"

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
    unsigned int verbosity = 2;
    int option = 0;
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

    att_set_verbose(verbosity);

    while((option = getopt_long(argc, argv, "f:hvV", long_options, &option_index)) != -1) {
        switch(option) {
            case 'f':
                filter = optarg;
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

    printf("Running tests with %s\n", filter ? filter : "no filter");
    att_run_tests(filter);
    bool all_valid = att_report() == 0;

    return all_valid ? 0 : -1;
}
