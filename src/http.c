/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "warudo.h"

/*#define WRD_CHECK_CONNECTION(request) \
    if(request.in == NULL || \
        request.out == NULL || \
        request.err == NULL) { \
    }*/
#define WRD_CHECK_CONNECTION(config) \
    if(config->client_fd <= 0) { \
        return WRD_ERROR; \
    }

WRD_API wrd_code wrd_http_buffer_puts(warudo *config, wrd_buffer *buffer, const char *str);
WRD_API wrd_code wrd_http_buffer_printf(warudo *config, wrd_buffer *buffer, const char *format, ...);

WRD_API wrd_code wrd_http_status(warudo *config, const char *status) {
    WRD_CHECK_CONNECTION(config)

    wrd_http_buffer_printf(config, &config->net_headers_buffer, u8"HTTP/1.1 %s\r\n", status);

    return WRD_OK;
}

WRD_API wrd_code wrd_http_additional(warudo *config) {
    WRD_CHECK_CONNECTION(config)

    char date_string[50];
    time_t current_time;
    struct tm *timeinfo;

    time(&current_time);
    timeinfo = gmtime(&current_time); // UTC time

    strftime(date_string, sizeof(date_string), u8"Date: %a, %d %b %Y %H:%M:%S GMT\r\n", timeinfo);
    wrd_http_buffer_puts(config, &config->net_headers_buffer, u8"Cache-Control: no-cache\r\n");
    wrd_http_buffer_puts(config, &config->net_headers_buffer, date_string);
    wrd_http_buffer_printf(config, &config->net_headers_buffer, u8"Server: %s %s\r\n", WRD_NAME, WRD_VERSION);

    return WRD_OK;
}

WRD_API wrd_code wrd_http_content_type(warudo *config, const char *content_type) {
     WRD_CHECK_CONNECTION(config)

    if(config->access_origin != NULL) {
        wrd_http_buffer_printf(config, &config->net_headers_buffer, u8"Access-Control-Allow-Origin: %s\r\n",
            config->access_origin);
    }

    wrd_http_buffer_printf(config, &config->net_headers_buffer, u8"Content-type: %s;charset=utf-8\r\n", content_type);

    return WRD_OK;
}

WRD_API wrd_code wrd_http_header(warudo *config, const char *status, const char *content_type) {
    WRD_CHECK_CONNECTION(config)

    wrd_http_status(config, status);
    wrd_http_content_type(config, content_type);
    wrd_http_additional(config);

    return WRD_OK;
}

WRD_API wrd_code wrd_http_ok(warudo *config) {
    WRD_CHECK_CONNECTION(config)

    return wrd_http_header(config, "200 OK", "application/json");
}

WRD_API wrd_code wrd_http_created(warudo *config, unsigned long long int id) {
    WRD_CHECK_CONNECTION(config)

    wrd_http_header(config, "201 Created", "application/json");
    wrd_http_printf(config, u8"{\"status\":\"success\",\"id\":%lld}", id);

    return WRD_OK;
}

WRD_API wrd_code wrd_http_multi_created(warudo *config, unsigned long int count) {
    WRD_CHECK_CONNECTION(config)

    wrd_http_header(config, "201 Created", "application/json");
    wrd_http_printf(config, u8"{\"status\":\"success\",\"count\":%lld}", count);

    return WRD_OK;
}

WRD_API wrd_code wrd_http_not_allowed(warudo *config, const char *allowed) {
    WRD_CHECK_CONNECTION(config)

    wrd_http_header(config, "405 Method Not Allowed", "text/type");
    wrd_http_buffer_printf(config, &config->net_headers_buffer, u8"Allow: %s\r\n", allowed);
    wrd_http_puts(config, u8"Not allowed.");

    return WRD_HTTP_NOT_ALLOWED;
}

WRD_API wrd_code wrd_http_server_error(warudo *config, const char *description) {
    WRD_CHECK_CONNECTION(config)

    wrd_http_header(config, "500 Internal Server Error", "text/plain");
    wrd_http_puts(config, description);

    return WRD_HTTP_INTERNAL_ERROR;
}

WRD_API wrd_code wrd_http_bad_request(warudo *config, const char *description) {
    WRD_CHECK_CONNECTION(config)

    wrd_http_header(config, "400 Bad Request", "application/json");
    wrd_http_printf(config, u8"{\"status\":\"failure\",\"error\":\"%s\"}", description);

    return WRD_HTTP_BAD_REQUEST;
}

WRD_API wrd_code wrd_http_not_found(warudo *config) {
    WRD_CHECK_CONNECTION(config)

    wrd_http_header(config, "404 Not Found", "text/plain");
    wrd_http_puts(config, u8"Unknown.");

    return WRD_HTTP_NOT_FOUND;
}

WRD_API wrd_code wrd_http_buffer_puts(warudo *config, wrd_buffer *buffer, const char *str) {
    WRD_CHECK_CONNECTION(config)

    char *str_buffer = buffer->buffer + buffer->position;
    size_t buffer_size = buffer->size - buffer->position - 1;
    size_t str_length = strlen(str);

    memcpy(str_buffer, str, str_length < buffer_size ? str_length : buffer_size);
    buffer->position += str_length;

    return WRD_OK;
}

WRD_API wrd_code wrd_http_buffer_printf(warudo *config, wrd_buffer *buffer, const char *format, ...) {
    WRD_CHECK_CONNECTION(config)

    char *str_buffer = buffer->buffer + buffer->position;
    size_t buffer_size = buffer->size - buffer->position - 1;

    va_list args;

    va_start(args, format);
    int written = vsnprintf(str_buffer, buffer_size, format, args);
    va_end(args);

    buffer->position += written;

    return WRD_OK;
}

WRD_API wrd_code wrd_http_puts(warudo *config, const char *str) {
    WRD_CHECK_CONNECTION(config)

    char *str_buffer = config->net_buffer.buffer + config->net_buffer.position;
    size_t buffer_size = config->net_buffer.size - config->net_buffer.position - 1;
    size_t str_length = strlen(str);

    memcpy(str_buffer, str, str_length < buffer_size ? str_length : buffer_size);
    config->net_buffer.position += str_length;

    return WRD_OK;
}

WRD_API wrd_code wrd_http_printf(warudo *config, const char *format, ...) {
    WRD_CHECK_CONNECTION(config)

    char *str_buffer = config->net_buffer.buffer + config->net_buffer.position;
    size_t buffer_size = config->net_buffer.size - config->net_buffer.position - 1;

    va_list args;

    va_start(args, format);
    int written = vsnprintf(str_buffer, buffer_size, format, args);
    va_end(args);

    config->net_buffer.position += written;

    return WRD_OK;
}

WRD_API wrd_code wrd_http_parse_query_headers(warudo *config) {
    CHECK_CONFIG

    // Reset parsed request state.
    config->request_method = WRD_REQUEST_UNKNOWN;
    config->request_path[0] = '\0';
    config->request_query_string[0] = '\0';
    config->request_headers_count = 0;
    config->request_body_offset = 0;
    config->page = WRD_PAGE_NOT_FOUND;
    config->http_version_major = 0;
    config->http_version_minor = 0;

    char *buf = config->net_input_buffer.buffer;
    unsigned int buf_len = config->net_input_buffer.position;

    if(!buf || buf_len == 0) {
        return WRD_ERROR;
    }

    // Find end of request line (\r\n).
    char *line_end = strstr(buf, "\r\n");

    if(!line_end) {
        return WRD_ERROR;
    }

    unsigned int request_line_len = line_end - buf;

    // Parse method.
    char *pos = buf;
    char *space = memchr(pos, ' ', request_line_len);

    if(!space) {
        return WRD_ERROR;
    }

    unsigned int method_len = space - pos;

    if(method_len == 3 && strncmp(pos, "GET", 3) == 0) {
        config->request_method = WRD_REQUEST_GET;
    } else if(method_len == 4 && strncmp(pos, "POST", 4) == 0) {
        config->request_method = WRD_REQUEST_POST;
    } else if(method_len == 4 && strncmp(pos, "HEAD", 4) == 0) {
        config->request_method = WRD_REQUEST_HEAD;
    } else if(method_len == 3 && strncmp(pos, "PUT", 3) == 0) {
        config->request_method = WRD_REQUEST_PUT;
    } else if(method_len == 6 && strncmp(pos, "DELETE", 6) == 0) {
        config->request_method = WRD_REQUEST_DELETE;
    } else if(method_len == 7 && strncmp(pos, "OPTIONS", 7) == 0) {
        config->request_method = WRD_REQUEST_OPTIONS;
    }

    // Parse URI (path and optional query string).
    pos = space + 1;
    char *uri_end = memchr(pos, ' ', line_end - pos);

    if(!uri_end) {
        return WRD_ERROR;
    }

    unsigned int uri_len = uri_end - pos;
    char *query_start = memchr(pos, '?', uri_len);

    if(query_start) {
        unsigned int path_len = query_start - pos;

        if(path_len >= WRD_MAX_PATH) {
            path_len = WRD_MAX_PATH - 1;
        }

        memcpy(config->request_path, pos, path_len);
        config->request_path[path_len] = '\0';

        unsigned int qs_len = uri_end - (query_start + 1);

        if(qs_len >= WRD_MAX_PATH) {
            qs_len = WRD_MAX_PATH - 1;
        }

        memcpy(config->request_query_string, query_start + 1, qs_len);
        config->request_query_string[qs_len] = '\0';
    } else {
        if(uri_len >= WRD_MAX_PATH) {
            uri_len = WRD_MAX_PATH - 1;
        }

        memcpy(config->request_path, pos, uri_len);
        config->request_path[uri_len] = '\0';
    }

    // Parse HTTP version.
    pos = uri_end + 1;

    if(pos + 8 <= line_end && strncmp(pos, "HTTP/", 5) == 0) {
        config->http_version_major = pos[5] - '0';

        if(pos[6] == '.') {
            config->http_version_minor = pos[7] - '0';
        }
    }

    // Parse headers.
    pos = line_end + 2; // Skip past \r\n.

    while(pos < buf + buf_len && config->request_headers_count < WRD_MAX_HEADERS) {
        // Check for end of headers (\r\n\r\n).
        if(pos[0] == '\r' && pos + 1 < buf + buf_len && pos[1] == '\n') {
            config->request_body_offset = (pos + 2) - buf;
            break;
        }

        char *header_end = strstr(pos, "\r\n");

        if(!header_end) {
            break;
        }

        // Find the colon separator.
        char *colon = memchr(pos, ':', header_end - pos);

        if(!colon) {
            pos = header_end + 2;
            continue;
        }

        unsigned int name_len = colon - pos;

        if(name_len >= WRD_MAX_HEADER_NAME) {
            name_len = WRD_MAX_HEADER_NAME - 1;
        }

        wrd_http_header_entry *entry = &config->request_headers[config->request_headers_count];
        memcpy(entry->name, pos, name_len);
        entry->name[name_len] = '\0';

        // Skip colon and optional whitespace.
        char *val_start = colon + 1;

        while(val_start < header_end && *val_start == ' ') {
            ++val_start;
        }

        unsigned int val_len = header_end - val_start;

        if(val_len >= WRD_MAX_HEADER_VALUE) {
            val_len = WRD_MAX_HEADER_VALUE - 1;
        }

        memcpy(entry->value, val_start, val_len);
        entry->value[val_len] = '\0';

        ++config->request_headers_count;
        pos = header_end + 2;
    }

    // Determine page from path.
    if(strcmp(config->request_path, "/") == 0) {
        config->page = WRD_PAGE_ROOT;
    } else if(strcmp(config->request_path, "/app/entries") == 0) {
        config->page = WRD_PAGE_APP;
    } else if(strcmp(config->request_path, "/app/keys") == 0) {
        config->page = WRD_PAGE_APP_KEYS;
    } else if(strcmp(config->request_path, "/app/dashboards") == 0) {
        config->page = WRD_PAGE_APP_VIEWS;
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_http_get_header(warudo *config, const char *name, char **value) {
    CHECK_CONFIG

    if(!name || !value) {
        return WRD_ERROR;
    }

    for(int i = 0; i < config->request_headers_count; ++i) {
        if(strcasecmp(config->request_headers[i].name, name) == 0) {
            *value = config->request_headers[i].value;

            return WRD_OK;
        }
    }

    *value = NULL;

    return WRD_HTTP_NOT_FOUND;
}

WRD_API wrd_code wrd_http_flush(warudo *config) {
    WRD_CHECK_CONNECTION(config)

    wrd_http_buffer_printf(config, &config->net_headers_buffer, "Content-Length: %u\r\n\r\n",
        config->net_buffer.position);

    wrd_net_send(config, &config->net_headers_buffer);
    wrd_net_send(config, &config->net_buffer);

    return WRD_OK;
}
