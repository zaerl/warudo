#include <stdio.h>
#include <string.h>

#include "http.h"
#include "log.h"
#include "net.h"
#include "timing.h"

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

    return WRD_OK;
}

WRD_API wrd_code wrd_http_get_header(warudo *config, const char *name, char **value) {
    CHECK_CONFIG

    return WRD_OK;
}

WRD_API wrd_code wrd_http_flush(warudo *config) {
    WRD_CHECK_CONNECTION(config)

    wrd_http_buffer_printf(config, &config->net_headers_buffer, "Content-Length: %u\r\n\r\n",
        config->net_buffer.position);

    wrd_net_send(config, &config->net_headers_buffer);
    wrd_net_send(config, &config->net_buffer);

    return WRD_OK;
}
