#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <time.h>
#include <getopt.h>

// Server URL
#define DEFAULT_SERVER_URL "http://localhost:6252/app/entries?multi=1"
#define VERSION "0.1.1"

// CLI commands
#define CLI_DEBUG 0
#define CLI_ENTRIES_PER_REQUEST 1
#define CLI_HELP 2
#define CLI_MAX_REQUESTS 3
#define CLI_VERBOSE 4

// CLI defaults
#define DEFAULT_DEBUG 0
#define DEFAULT_ENTRIES_PER_REQUEST 600
#define DEFAULT_MAX_REQUESTS -1
#define DEFAULT_VERBOSE 0

// Callback function to write response data
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    (void)ptr; // Discard response data
    (void)userdata; // Discard response data

    // Discard response data
    return size * nmemb;
}

void debug_dump(const char* text, FILE* stream, unsigned char* ptr, size_t size) {
    size_t i;
    size_t c;
    unsigned int width = 0x10;

    fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n", text, (long)size, (long)size);

    for(i = 0; i < size; i += width) {
        fprintf(stream, "%4.4lx: ", (long)i);

        // Show hex to the left
        for(c = 0; c < width; ++c) {
            if(i + c < size) {
                fprintf(stream, "%02x ", ptr[i+c]);
            } else {
                fputs("   ", stream);
            }
        }

        // Show data on the right
        for(c = 0; (c < width) && (i + c < size); ++c) {
            char x = (ptr[i + c] >= 0x20 && ptr[i + c] < 0x80) ? ptr[i + c] : '.';
            fputc(x, stream);
        }

        fputc('\n', stream);
    }
}

int debug_trace(CURL* handle, curl_infotype type, char* data, size_t size, void* clientp) {
    const char* text;
    (void)handle;
    (void)clientp;

    switch (type) {
        case CURLINFO_TEXT:
            fputs("== Info: ", stderr);
            fwrite(data, size, 1, stderr);

        default:
            return 0;

        case CURLINFO_HEADER_OUT:
            text = "=> Send header";
            break;
        case CURLINFO_DATA_OUT:
            text = "=> Send data";
            break;
        case CURLINFO_SSL_DATA_OUT:
            text = "=> Send SSL data";
            break;
        case CURLINFO_HEADER_IN:
            text = "<= Recv header";
            break;
        case CURLINFO_DATA_IN:
            text = "<= Recv data";
            break;
        case CURLINFO_SSL_DATA_IN:
            text = "<= Recv SSL data";
            break;
    }

    debug_dump(text, stderr, (unsigned char*)data, size);
    return 0;
}

double send_request(CURL* curl) {
    struct timespec start, end;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);
    CURLcode res = curl_easy_perform(curl);
    clock_gettime(CLOCK_MONOTONIC, &end);

    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    if (res == CURLE_OK) {
        putc('.', stdout);
    } else {
        fprintf(stderr, "Error sending request: %s\n", curl_easy_strerror(res));
    }

    return elapsed;
}

int show_version(void) {
    printf("warudo v%s\n", VERSION);

    return 0;
}

void show_help(const char *executable, struct option options[], const char *descriptions[]) {
    printf("Usage: %s [OPTIONS] FILENAME\n", executable);
    puts("Options:");

    for(unsigned long i = 0; i < 6; ++i) {
        printf("  -%c%s, --%s%s\n\t%s\n",
            options[i].val,
            options[i].has_arg == no_argument ? "" : " \033[4mnumber\033[0m",
            options[i].name,
            options[i].has_arg == no_argument ? "" : "=\033[4mnumber\033[0m",
            descriptions[i]);
    }
}

int main(int argc, char* argv[]) {
    char *filename = NULL;
    char* line = NULL;
    curl_mime* mime = NULL;
    CURL* curl = NULL;
    double elapsed = 0;
    FILE* file = NULL;
    size_t len = 0;
    ssize_t read = 0;
    unsigned long entities_created = 0;
    unsigned long lines_read = 0;
    unsigned long lines_scanned = 0;
    unsigned long lines_skipped = 0;
    unsigned long requests = 0;

    int debug_flag = DEFAULT_DEBUG;
    int entries_per_request = DEFAULT_ENTRIES_PER_REQUEST;
    int max_requests = DEFAULT_MAX_REQUESTS;
    int verbose_flag = DEFAULT_VERBOSE;
    char *url = DEFAULT_SERVER_URL;

    struct option long_options[] = {
        { "debug", no_argument, 0, 'd' },
        { "entries", required_argument, 0, 'e' },
        { "help", no_argument, 0, 'h' },
        { "max-requests", required_argument, 0, 'm' },
        { "url", required_argument, 0, 'u' },
        { "verbose", no_argument, 0, 'v' },
        { "version", no_argument, 0, 'V' },
        { 0, 0, 0, 0 }
    };
    const char *descriptions[] = {
        "Debug output",
        "Number of entries per request",
        "Show this help",
        "Maximum number of requests",
        "Server URL",
        "Verbose output",
        "Print version"
    };

    if(argc == 1) {
        fprintf(stderr, "Error: Missing filename argument.\n");
        show_help(argv[0], long_options, descriptions);

        return 1;
    }

    int option;
    int option_index = 0;

    while((option = getopt_long(argc, argv, "de:hm:u:vV", long_options, &option_index)) != -1) {
        switch (option) {
            case 'd':
                debug_flag = 1;
                break;
            case 'e':
                entries_per_request = atoi(optarg);
                break;
            case 'h':
                show_help(argv[0], long_options, descriptions);
                return 0;
            case 'm':
                max_requests = atoi(optarg);
                break;
            case 'u':
                url = optarg;
                break;
            case 'v':
                verbose_flag = 1;
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

    // Process other non-option arguments (if any)
    if(optind >= argc) {
        fprintf(stderr, "Error: Missing filename argument.\n");
        show_help(argv[0], long_options, descriptions);

        return 1;
    }

    filename = argv[optind];

    // Disable output buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    if(strcmp(filename, "-") == 0) {
        file = stdin;
    } else {
        file = fopen(filename, "r");

        if (!file) {
            fprintf(stderr, "File not found: %s\n", filename);

            return 1;
        }
    }

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(!curl) {
        fprintf(stderr, "Error initializing CURL\n");

        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_URL, url);

    if(verbose_flag) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }

    if(debug_flag) {
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_trace);
    }

    while((read = getline(&line, &len, file)) != -1) {
        ++lines_scanned;

        if(len == 0 || (len == 1 && line[0] == '\n')) {
            ++lines_skipped;

            continue;
        }

        if(lines_read == 0) {
            // Initialize the form
            mime = curl_mime_init(curl);
        }

        char fieldname[20];

        // Create an automatic field name
        snprintf(fieldname, sizeof(fieldname), "field%lu", lines_read + 1);

        // Add JSON
        curl_mimepart* part = curl_mime_addpart(mime);
        curl_mime_data(part, line, read);
        curl_mime_name(part, fieldname);
        ++lines_read;

        if(lines_read == (unsigned long)entries_per_request) {
            curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
            curl_easy_setopt(curl, CURLOPT_URL, url);
            ++requests;

            elapsed += send_request(curl);
            entities_created += lines_read;

            lines_read = 0;
            curl_mime_free(mime);

            if(max_requests >= 0 && requests == (unsigned long)max_requests) {
                break;
            }
        }
    }

    if(lines_read > 0) {
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        ++requests;

        elapsed += send_request(curl);
        entities_created += lines_read;

        curl_mime_free(mime);
    }

    printf("\n\n%lu requests of %lu entries\n%lu entities created\n%lu lines scanned (%lu skipped)\n",
        requests, (unsigned long)entries_per_request, entities_created, lines_scanned, lines_skipped);

    printf("Execution time %.4f seconds\n~%.4fs / request\n", elapsed, elapsed / requests);

    curl_easy_cleanup(curl);
    fclose(file);

    if(line) {
        free(line);
    }

    curl_global_cleanup();

    return 0;
}
