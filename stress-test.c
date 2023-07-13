#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <time.h>

// Server URL
#define SERVER_URL "http://localhost:6252/app/entries"

// Callback function to write response data
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    // Discard response data
    return size * nmemb;
}

int main(int argc, char *argv[]) {
    // Check if filename is provided
    if (argc < 2) {
        printf("Please provide a filename.\n");
        return 1;
    }

    // Open file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("File not found: %s\n", argv[1]);
        return 1;
    }

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    // Buffer for storing a line
    char line[1024];
    unsigned long long int line_count = 0;

    // Read file line by line
    while (fgets(line, sizeof(line), file)) {
        ++line_count;
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        // Send POST request using libcurl
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, SERVER_URL);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, line);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        res = curl_easy_perform(curl);
        clock_gettime(CLOCK_MONOTONIC, &end);

        // Check curl result
        if (res == CURLE_OK) {
            double time_taken = (end.tv_sec - start.tv_sec) * 1000.0;
            time_taken += (end.tv_nsec - start.tv_nsec) / 1000000.0;

            // Output a dot for a successful request
            if(time_taken > 1000) {
                printf("%lld (%.0lf ms)\n", line_count, time_taken);
            }
        } else {
            // Output an error message if the request fails
            printf("Error sending request: %s\n", line);
        }
    }

    // Clean up
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    fclose(file);

    // Output a new line after processing all lines
    printf("\n");

    return 0;
}
