/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "warudo.h"

WRD_API wrd_code wrd_parse_query_string(warudo *config, char *query_string) {
    CHECK_CONFIG

    // Reset to defaults.
    config->query.id = 0;
    config->query.offset = 0;
    config->query.limit = WRD_DEFAULT_QUERY_LIMIT;
    config->query.multi = WRD_DEFAULT_QUERY_MULTI;
    config->query.key = NULL;
    config->query.value = NULL;
    config->query.orderby = NULL;
    config->query.sort = NULL;

    if(query_string == NULL || query_string[0] == '\0') {
        return WRD_OK;
    }

    char *saveptr;
    char *parameter = strtok_r(query_string, "&", &saveptr);

    while(parameter != NULL) {
        char *delimiter = strchr(parameter, '=');

        if(delimiter != NULL) {
            int name_len = delimiter - parameter;
            char *value = delimiter + 1;

            if(name_len == 5 && strncmp(parameter, "limit", 5) == 0) {
                config->query.limit = (unsigned int)strtol(value, NULL, 10);
            } else if(name_len == 6 && strncmp(parameter, "offset", 6) == 0) {
                config->query.offset = strtoull(value, NULL, 10);
            } else if(name_len == 2 && strncmp(parameter, "id", 2) == 0) {
                config->query.id = strtoull(value, NULL, 10);
            } else if(name_len == 5 && strncmp(parameter, "multi", 5) == 0) {
                config->query.multi = (unsigned int)strtol(value, NULL, 10);
            } else if(name_len == 3 && strncmp(parameter, "key", 3) == 0) {
                config->query.key = strdup(value);
            } else if(name_len == 5 && strncmp(parameter, "value", 5) == 0) {
                config->query.value = strdup(value);
            } else if(name_len == 7 && strncmp(parameter, "orderby", 7) == 0) {
                config->query.orderby = strdup(value);
            } else if(name_len == 4 && strncmp(parameter, "sort", 4) == 0) {
                config->query.sort = strdup(value);
            }
        }

        parameter = strtok_r(NULL, "&", &saveptr);
    }

    return WRD_OK;
}

WRD_API char *wrd_url_decode(const char *input) {
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

WRD_API int wrd_is_valid_boundary(const char *boundary) {
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
WRD_API const char *wrd_get_formdata_boundary(const char *content_type) {
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

WRD_API wrd_code wrd_parse_formdata(const char *input, long int length, const char *boundary,
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

WRD_API long wrd_content_length(warudo *config) {
    if(!config) {
        return 0;
    }

    char *value = NULL;

    if(wrd_http_get_header(config, "Content-Length", &value) == WRD_OK && value) {
        return strtol(value, NULL, 10);
    }

    return 0;
}

WRD_API char *wrd_read_content(long int length, warudo *config) {
    long int len = length == 0 ? wrd_content_length(config) : length;

    if(len <= 0 || !config) {
        return NULL;
    }

    // Read body from the already-parsed input buffer.
    char *body_start = config->net_input_buffer.buffer + config->request_body_offset;
    unsigned int available = config->net_input_buffer.position - config->request_body_offset;

    if((unsigned long)len > available) {
        len = available;
    }

    char *data = malloc(len + 1);

    if(data) {
        memcpy(data, body_start, len);
        data[len] = '\0';
    }

    return data;
}
