#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <time.h>

// Server URL
#define SERVER_URL "http://localhost:6252/app/entries?multi=1"
#define MAX_LINES_PER_REQUEST 500
#define VERBOSE 0
#define DEBUG 0

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

int send_request(CURL* curl) {
    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        putc('.', stdout);

        return 1;
    } else {
        fprintf(stderr, "Error sending request: %s\n", curl_easy_strerror(res));
    }

    return 0;
}

int main(int argc, char* argv[]) {
    // Check if filename is provided
    if (argc < 2) {
        printf("Please provide a filename.\n");
        return 1;
    }

    // Disable output buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    char* line = NULL;
    CURL* curl;
    curl_mime* mime;
    FILE* file;
    unsigned long entities_created = 0;
    unsigned long lines_read = 0;
    unsigned long lines_skipped = 0;
    unsigned long lines_scanned = 0;
    unsigned long requests = 0;
    size_t len = 0;
    ssize_t read;

    file = fopen(argv[1], "r");
    if (!file) {
        printf("File not found: %s\n", argv[1]);
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

    if(VERBOSE) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }

    if(DEBUG) {
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_trace);
    }

    struct timespec start, end;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);

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

        if(lines_read == MAX_LINES_PER_REQUEST) {
            curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
            curl_easy_setopt(curl, CURLOPT_URL, SERVER_URL);
            ++requests;

            if(send_request(curl)) {
                entities_created += lines_read;
            }

            lines_read = 0;
            curl_mime_free(mime);
        }
    }

    if(lines_read > 0) {
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        ++requests;

        if(send_request(curl)) {
            entities_created += lines_read;
        }

        curl_mime_free(mime);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("\n\n%lu requests\n%lu entities created\n%lu lines scanned (%lu skipped)\nExecution time %.4f seconds\n",
        requests, entities_created, lines_scanned, lines_skipped, elapsed);

    curl_easy_cleanup(curl);
    fclose(file);

    if(line) {
        free(line);
    }

    curl_global_cleanup();

    return 0;
}
