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

#include "zaerl.h"

// Function to escape special characters in a string for HTML
char* zaerl_escape_html(const char* input) {
    size_t len = strlen(input);
    size_t escaped_len = len * 6 + 1;
    char* escaped = malloc(len);

    if (escaped == NULL) {
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

        if (j >= escaped_len - 6) {
            // Increase buffer size if necessary
            escaped_len *= 2;
            char* temp = realloc(escaped, escaped_len);

            if (temp == NULL) {
                free(escaped);

                return NULL;
            }

            escaped = temp;
        }
    }

    escaped[j] = '\0';

    return escaped;
}

int zaerl_content_type(const char* content_type, zaerl *config) {
    FCGX_FPrintF(config->request.out, "Content-type: %s;charset=utf-8\r\n\r\n", content_type);

    return 0;
}

int zaerl_not_allowed(const char* allowed, zaerl* config) {
    FCGX_FPrintF(config->request.out, "Status: 405 Method Not Allowed\r\nAllow: %s\r\n", allowed);
    zaerl_content_type("text/plain", config);

    FCGX_PutS("Not allowed.", config->request.out);

    return 405;
}

int zaerl_server_error(const char* description, zaerl* config) {
    FCGX_PutS("Status: 500 Internal Server Error\r\n", config->request.out);
    zaerl_content_type("text/plain", config);

    FCGX_PutS(description, config->request.out);

    return 500;
}

int zaerl_bad_request(const char* description, zaerl* config) {
    FCGX_PutS("Status: 400 Bad Request\r\n", config->request.out);
    zaerl_content_type("application/json", config);

    FCGX_FPrintF(config->request.out, "{\"status\":\"failure\",\"error\":\"%s\"}", description);

    return 400;
}

unsigned long zaerl_process_id(void) {
#ifdef _WIN32
    return (unsigned long)GetCurrentProcessId();
#elif __linux__ || __APPLE__
    return (unsigned long)getpid();
#endif
}

int zaerl_environ(zaerl* config) {
    FCGX_FPrintF(config->request.out, "<!--db: %s-->\n", ZAERL_DB_FILE);
    FCGX_FPrintF(config->request.out, "<!--config: %s-->\n", ZAERL_CONFIG_FILE);
    FCGX_FPrintF(config->request.out, "<!--pid: %lu-->\n", zaerl_process_id());

    return 0;
}

int zaerl_page_not_found(zaerl* config) {
    FCGX_PutS("Status: 404 Not Found\r\n", config->request.out);
    zaerl_content_type("text/plain", config);

    FCGX_PutS("Unknown.", config->request.out);

    return 404;
}
