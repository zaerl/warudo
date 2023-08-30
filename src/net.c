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

#include "net.h"
#include "fcgi.h"

// Function to escape special characters in a string for HTML
char *wrd_escape_html(const char *input) {
    if(input == NULL) {
        return NULL;
    }

    size_t len = strlen(input);
    size_t escaped_len = len * 6 + 1;
    char *escaped = malloc(len);

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
            char *temp = realloc(escaped, escaped_len);

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

char *wrd_url_decode(const char *input) {
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

                // combine the next to numbers into one
                hex_code[0] = decoded_url[i + 1];
                hex_code[1] = decoded_url[i + 2];

                // convert it to decimal
                long int x = strtol(hex_code, NULL, 16);

                // remove the hex
                memmove(&decoded_url[i + 1], &decoded_url[i + 3], strlen(&decoded_url[i + 3]) + 1);

                decoded_url[i] = x;
            }
        }
    }

    return decoded_url;
}

int wrd_is_valid_boundary(const char *boundary) {
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
const char *wrd_get_formdata_boundary(const char *content_type) {
    if(content_type == NULL || strlen(content_type) < 31) {
        return NULL;
    }

    const char *boundary = strstr(content_type, "multipart/form-data; boundary=");

    if(boundary == NULL || boundary != content_type) {
        return NULL;
    }

    boundary = content_type + 30;

    if(!wrd_is_valid_boundary(boundary)) {
        return NULL;
    }

    return boundary;
}

unsigned long wrd_process_id(void) {
#ifdef _WIN32
    return (unsigned long)GetCurrentProcessId();
#elif __linux__ || __APPLE__
    return (unsigned long)getpid();
#endif
}

wrd_code wrd_environ(warudo *config) {
    wrd_fcgi_printf(config, "<!--db: %s-->\n", WRD_DB_FILE);
    wrd_fcgi_printf(config, "<!--config: %s-->\n", WRD_CONFIG_FILE);
    wrd_fcgi_printf(config, "<!--pid: %lu-->\n", wrd_process_id());

    return WRD_OK;
}

long wrd_content_length(warudo *config) {
    char *length = wrd_fcgi_get_param("CONTENT_LENGTH", config);
    long int len = 0;

    if(length != NULL) {
        len = strtol(length, NULL, 10);
    }

    return len;
}


char *wrd_read_content(long int length, warudo *config) {
    long int len = length == 0 ? wrd_content_length(config) : length;

    if(len <= 0) {
        return NULL;
    }

    char *data = malloc(len);

    FCGX_GetStr(data, len, config->request.in);

    return data;
}

wrd_code wrd_parse_formdata(const char *input, long int length, const char *boundary,
    wrd_parse_formdata_callback callback, warudo *config) {
    char *full_boundary = NULL;
    long int index = 0;
    int res = 0;

    if(!config || length == 0 || input == NULL || boundary == NULL || callback == NULL) {
        return WRD_PARSER_EMPTY;
    }

    long int boundary_length = strlen(boundary);

    if(boundary_length == 0) {
        return WRD_PARSER_EMPTY_BOUNDARY;
    }

    long int full_boundary_length = boundary_length + 4;
    full_boundary = malloc(full_boundary_length + 1);

    full_boundary[0] = '\r';
    full_boundary[1] = '\n';
    full_boundary[2] = '-';
    full_boundary[3] = '-';
    full_boundary[full_boundary_length] = '\0';

    strcpy(full_boundary + 4, boundary);

    // At least one entry
    if(length < boundary_length * 2 + 54) {
        res = WRD_PARSER_VOID;
        goto error;
    }

    while(index < length) {
        char *position = strstr(input + index, index ? full_boundary : full_boundary + 2);

        if(position == NULL) {
            res = WRD_PARSER_NO_BOUNDARY;
            goto error;
        }

        if(index && position != input + index) {
            int json_length = position - (input + index);

            if((*callback)(input + index, json_length, config) == WRD_OK) {
                ++res;
            }

            if(length - ((position - input) + full_boundary_length) == 4) {
                // --{boundary}--\r\n
                //             ^^
                if(strncmp(input + index + json_length + full_boundary_length, "--\r\n", 4) == 0) {
                    break;
                } else {
                    res = WRD_PARSER_MISSING_END;
                    goto error;
                }
            }

            index += json_length + full_boundary_length;
        } else {
            index += index ? full_boundary_length : full_boundary_length - 2;
        }

        position = strstr(input + index, "\r\nContent-Disposition: form-data; name=\"");

        if(position == NULL || position != input + index) {
            res = WRD_PARSER_MISSING_CONTENT;
            goto error;
        }

        index += 40;

        // --{boundary}\r\n
        // Content-Disposition: form-data; name="{field}"\r\n
        //                                       ^^
        position = strstr(input + index, "\"\r\n\r\n");

        if(position == NULL) {
            res = WRD_PARSER_MISSING_BODY;
            goto error;
        }

        // --{boundary}\r\n
        // Content-Disposition: form-data; name="{field}"\r\n
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
