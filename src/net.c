#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcgiapp.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>

extern char **environ;
#endif

#include "warudo.h"

// Function to escape special characters in a string for HTML
char* warudo_escape_html(const char* input) {
    size_t len = strlen(input);
    size_t escaped_len = len * 6 + 1;
    char* escaped = malloc(len);

    if(escaped == NULL) {
        return NULL;
    }

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        switch (input[i]) {
            case '&':
                strncpy(&escaped[j], "&amp;", 5);
                j += 5;
                break;
            case '<':
                strncpy(&escaped[j], "&lt;", 4);
                j += 4;
                break;
            case '>':
                strncpy(&escaped[j], "&gt;", 4);
                j += 4;
                break;
            case '"':
                strncpy(&escaped[j], "&quot;", 6);
                j += 6;
                break;
            case '\'':
                strncpy(&escaped[j], "&#39;", 5);
                j += 5;
                break;
            default:
                escaped[j] = input[i];
                j++;
                break;
        }

        if(j >= escaped_len - 6) {
            // Increase buffer size if necessary
            escaped_len *= 2;
            char* temp = realloc(escaped, escaped_len);

            if(temp == NULL) {
                free(escaped);

                return NULL;
            }

            escaped = temp;
        }
    }

    escaped[j] = '\0';

    return escaped;
}

int warudo_status(const char* status, warudo* config) {
    FCGX_FPrintF(config->request.out, "Status: %s\r\n", status);

    return 0;
}

int warudo_content_type(const char* content_type, warudo *config) {
    if(config->access_origin != NULL) {
        FCGX_FPrintF(config->request.out, "Access-Control-Allow-Origin: %s\r\n", config->access_origin);
    }

    FCGX_FPrintF(config->request.out, "Content-type: %s;charset=utf-8\r\n\r\n", content_type);

    return 0;
}

int warudo_header(const char* status, const char* content_type, warudo* config) {
    warudo_status(status, config);
    warudo_content_type(content_type, config);

    return 0;
}

int warudo_not_allowed(const char* allowed, warudo* config) {
    warudo_status("405 Method Not Allowed", config);
    FCGX_FPrintF(config->request.out, "Allow: %s\r\n", allowed);
    warudo_content_type("text/type", config);
    FCGX_PutS("Not allowed.", config->request.out);

    return 405;
}

int warudo_server_error(const char* description, warudo* config) {
    warudo_header("500 Internal Server Error", "text/plain", config);
    FCGX_PutS(description, config->request.out);

    return 500;
}

int warudo_bad_request(const char* description, warudo* config) {
    warudo_header("400 Bad Request", "application/json", config);
    FCGX_FPrintF(config->request.out, "{\"status\":\"failure\",\"error\":\"%s\"}", description);

    return 400;
}

int warudo_page_not_found(warudo* config) {
    warudo_header("404 Not Found", "text/plain", config);
    FCGX_PutS("Unknown.", config->request.out);

    return 404;
}


unsigned long warudo_process_id(void) {
#ifdef _WIN32
    return (unsigned long)GetCurrentProcessId();
#elif __linux__ || __APPLE__
    return (unsigned long)getpid();
#endif
}

int warudo_environ(warudo* config) {
    FCGX_FPrintF(config->request.out, "<!--db: %s-->\n", WARUDO_DB_FILE);
    FCGX_FPrintF(config->request.out, "<!--config: %s-->\n", WARUDO_CONFIG_FILE);
    FCGX_FPrintF(config->request.out, "<!--pid: %lu-->\n", warudo_process_id());

    return 0;
}

long int warudo_content_length(warudo* config) {
    char *length = FCGX_GetParam("CONTENT_LENGTH", config->request.envp);
    long int len = 0;

    if(length != NULL) {
        len = strtol(length, NULL, 10);
    }

    return len;
}


char* warudo_read_content(warudo* config, long int length) {
    long int len = length == 0 ? warudo_content_length(config) : length;

    if(len <= 0) {
        return NULL;
    }

    char *data = malloc(len + 1);

    FCGX_GetStr(data, len, config->request.in);
    data[len] = '\0';

    return data;
}
