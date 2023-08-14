#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>

extern char **environ;
#endif

#include "warudo.h"

#define WARUDO_CHECK_CONNECTION(request) \
    if(request.in == NULL || \
        request.out == NULL || \
        request.err == NULL) { \
        return WARUDO_FCGI_INIT_ERROR; \
    }

// Function to escape special characters in a string for HTML
char* warudo_escape_html(const char* input) {
    if(input == NULL) {
        return NULL;
    }

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

char* warudo_url_decode(const char* input) {
    if(input == NULL) {
        return NULL;
    }

    int decoded = 0;
    char *decoded_url = malloc(strlen(input) + 1);
    char hex_code[] = "00";

    strcpy(decoded_url, input);

    while(!decoded) {
        decoded = 1;
        unsigned long i;

        for(i = 0; i < strlen(decoded_url); ++i) {
            if(decoded_url[i] != '%') {
                continue;
            }

            if(decoded_url[i + 1] == 0) {
                return decoded_url;
            }

            if(isxdigit(decoded_url[i + 1]) && isxdigit(decoded_url[i + 2])) {
                decoded = 0;

                /* combine the next to numbers into one */
                hex_code[0] = decoded_url[i + 1];
                hex_code[1] = decoded_url[i + 2];

                /* convert it to decimal */
                long int x = strtol(hex_code, NULL, 16);

                /* remove the hex */
                memmove(&decoded_url[i + 1], &decoded_url[i + 3], strlen(&decoded_url[i + 3]) + 1);

                decoded_url[i] = x;
            }
        }
    }

    return decoded_url;
}

int is_valid_boundary(const char* boundary) {
    if(boundary == NULL) {
        return 0;
    }

    int count = 0;

    while(*boundary) {
        char c = *boundary;

        if (!(c == '-' || c == '_' || isalnum(c))) {
            return 0;
        }

        ++boundary;
        ++count;
    }

    return count ? 1 : 0;
}

// multipart/form-data; boundary=random string
char* warudo_get_formdata_boundary(char* content_type) {
    if(content_type == NULL || strlen(content_type) < 31) {
        return NULL;
    }

    char* boundary = strstr(content_type, "multipart/form-data; boundary=");

    if(boundary == NULL || boundary != content_type) {
        return NULL;
    }

    boundary = content_type + 30;

    if(!is_valid_boundary(boundary)) {
        return NULL;
    }

    return boundary;
}

int warudo_status(const char* status, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    FCGX_FPrintF(config->request.out, "Status: %s\r\n", status);

    return WARUDO_OK;
}

int warudo_content_type(const char* content_type, warudo *config) {
    WARUDO_CHECK_CONNECTION(config->request);

    if(config->access_origin != NULL) {
        FCGX_FPrintF(config->request.out, "Access-Control-Allow-Origin: %s\r\n", config->access_origin);
    }

    FCGX_FPrintF(config->request.out, "Content-type: %s;charset=utf-8\r\n\r\n", content_type);

    return WARUDO_OK;
}

int warudo_header(const char* status, const char* content_type, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_status(status, config);
    warudo_content_type(content_type, config);

    return WARUDO_OK;
}

int warudo_ok(warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_header("200 OK", "application/json", config);

    return WARUDO_OK;
}

int warudo_created(unsigned long long int id, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_header("200 OK", "application/json", config);
    FCGX_FPrintF(config->request.out, "{\"status\":\"success\",\"id\":%lld}", id);

    return WARUDO_OK;
}

int warudo_not_allowed(const char* allowed, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_status("405 Method Not Allowed", config);
    FCGX_FPrintF(config->request.out, "Allow: %s\r\n", allowed);
    warudo_content_type("text/type", config);
    FCGX_PutS("Not allowed.", config->request.out);

    return WARUDO_HTTP_NOT_ALLOWED;
}

int warudo_server_error(const char* description, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_header("500 Internal Server Error", "text/plain", config);
    FCGX_PutS(description, config->request.out);

    return WARUDO_HTTP_INTERNAL_ERROR;
}

int warudo_bad_request(const char* description, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_header("400 Bad Request", "application/json", config);
    FCGX_FPrintF(config->request.out, "{\"status\":\"failure\",\"error\":\"%s\"}", description);

    return WARUDO_HTTP_BAD_REQUEST;
}

int warudo_page_not_found(warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_header("404 Not Found", "text/plain", config);
    FCGX_PutS("Unknown.", config->request.out);

    return WARUDO_HTTP_NOT_FOUND;
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

    return WARUDO_OK;
}

long int warudo_content_length(warudo* config) {
    char *length = FCGX_GetParam("CONTENT_LENGTH", config->request.envp);
    long int len = 0;

    if(length != NULL) {
        len = strtol(length, NULL, 10);
    }

    return len;
}


char* warudo_read_content(long int length, warudo* config) {
    long int len = length == 0 ? warudo_content_length(config) : length;

    if(len <= 0) {
        return NULL;
    }

    char *data = malloc(len);

    FCGX_GetStr(data, len, config->request.in);

    return data;
}

int warudo_parse_formdata(const char* input, long int length, const char* boundary) {
    char *full_boundary = NULL;
    int res = WARUDO_READ_ERROR;

    if(length == 0 || input == NULL || boundary == NULL) {
        goto error;
    }

    long int index = 0;
    long int boundary_length = strlen(boundary);
    long int full_boundary_length = boundary_length + 2;
    full_boundary = malloc(full_boundary_length + 1);

    full_boundary[0] = '-';
    full_boundary[1] = '-';
    full_boundary[full_boundary_length] = '\0';

    strcpy(full_boundary + 2, boundary);

    // At least one entry
    if(length < boundary_length * 2 + 54 || input[0] != '-' || input[1] != '-') {
        return WARUDO_EMPTY_CONTENT_ERROR;
    }

    while(index < length) {
        char* position = strstr(input + index, full_boundary);

        if(position == NULL) {
            goto error;
        }

        if(index && position != input + index) {
            int json_length = position - (input + index);

            printf("FOUND JSON >>>%.*s<<<\n", json_length, input + index);

            if(length - ((position - input) + full_boundary_length) == 4) {
                // --{boundary}--\r\n
                //             ^^
                if(strncmp(input + index + json_length + full_boundary_length, "--\r\n", 4) == 0) {
                    res = WARUDO_OK;
                    break;
                } else {
                    goto error;
                }
            }

            index += json_length + full_boundary_length;
        } else {
            index += full_boundary_length;
        }

        position = strstr(input + index, "\r\nContent-Disposition: form-data; name=\"");

        if(position == NULL || position != input + index) {
            goto error;
        }

        index += 40;

        // --{boundary}\r\n
        // Content-Disposition: form-data; name="{field}"\r\n
        //                                       ^^
        position = strstr(input + index, "\"\r\n\r\n");

        if(position == NULL) {
            // printf("NO newlines\n");
            goto error;
        }

        // --{boundary}\r\n
        // Content-Disposition: form-data; name="{field}"\r\n
        // \r\n
        // \r\n
        // {json}\r\n
        // ^^
        index += (position - (input + index)) + 5;
    }

error:
    if(full_boundary) {
        free(full_boundary);
    }

    return res;
}
