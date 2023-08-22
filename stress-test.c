#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <time.h>

// Server URL
#define SERVER_URL "http://localhost:6252/app/entries?multi=1"
#define ENTRIES_PER_REQUEST_DEFAULT 500
#define MAX_REQUESTS_DEFAULT -1
#define VERBOSE_DEFAULT 0
#define DEBUG_DEFAULT 0

#define CLI_ARG(name) "\033[4m" name "\033[0m"

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

long get_cli_number(char *arg) {
    if(arg == NULL) {
        return -1;
    }

    char* endptr = NULL;
    long number = strtol(arg, &endptr, 10);

    if(*endptr != '\0') {
        return -1;
    }

    return number;
}

int main(int argc, char* argv[]) {
    char *filename = NULL;
    char* line = NULL;
    curl_mime* mime = NULL;
    CURL* curl = NULL;
    double elapsed = 0;
    FILE* file = NULL;
    int debug = DEBUG_DEFAULT;
    int verbose = VERBOSE_DEFAULT;
    long max_requests = MAX_REQUESTS_DEFAULT;
    long entries_per_request = ENTRIES_PER_REQUEST_DEFAULT;
    size_t len = 0;
    ssize_t read = 0;
    unsigned long entities_created = 0;
    unsigned long lines_read = 0;
    unsigned long lines_scanned = 0;
    unsigned long lines_skipped = 0;
    unsigned long requests = 0;

    if(argc == 1 || (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))) {
        printf("Usage: %s [options] filename\n", argv[0]);
        puts("Options:");
        puts("  -D, --debug\tDebug output");
        puts("  -h, --help\tShow this help");
        puts("  -m " CLI_ARG("number") "\tMaximum number of requests");
        puts("  -r " CLI_ARG("number") "\tMaximum number of entries per request");
        puts("  -v, --verbose\tVerbose output");

        return 0;
    }

    for(int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if(strcmp(argv[i], "-D") == 0 || strcmp(argv[i], "--debug") == 0) {
            debug = 1;
        } else if(strcmp(argv[i], "-m") == 0) {
            if(i + 1 >= argc) {
                printf("Please provide a number for the maximum number of requests.\n");

                return 1;
            }

            i += 1;

            max_requests = get_cli_number(argv[i]);

            if(max_requests < 0) {
                printf("Please provide a valid number for the maximum number of requests.\n");

                return 1;
            }
        } else if(strcmp(argv[i], "-r") == 0) {
            if(i + 1 >= argc) {
                printf("Please provide a number for the maximum number of entries per request.\n");

                return 1;
            }

            i += 1;

            entries_per_request = get_cli_number(argv[i]);

            if(entries_per_request < 0) {
                printf("Please provide a valid number for the maximum number of entries per request.\n");

                return 1;
            }
        } else {
            filename = argv[i];
        }
    }

    // Check if filename is provided
    if(filename == NULL) {
        printf("Please provide a filename.\n");
        return 1;
    }

    // Disable output buffering
    setvbuf(stdout, NULL, _IONBF, 0);
    file = fopen(filename, "r");

    if (!file) {
        printf("File not found: %s\n", filename);
        return 1;
    }

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(!curl) {
        fprintf(stderr, "Error initializing CURL\n");
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_URL, SERVER_URL);

    if(verbose) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }

    if(debug) {
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
            curl_easy_setopt(curl, CURLOPT_URL, SERVER_URL);
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
