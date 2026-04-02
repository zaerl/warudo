#include "test.h"

// Helper to set up a mock HTTP request in the input buffer.
static void mock_request(warudo *config, const char *raw_request) {
    unsigned int len = strlen(raw_request);

    if(len > config->net_input_buffer.size) {
        len = config->net_input_buffer.size;
    }

    memset(config->net_input_buffer.buffer, 0, config->net_input_buffer.size);
    memcpy(config->net_input_buffer.buffer, raw_request, len);
    config->net_input_buffer.position = len;
}

void *test_http(void *arg) {
    MOCK_CONFIG

    // Allocate a small input buffer for testing.
    config.net_input_buffer.size = 4096;
    config.net_input_buffer.buffer = calloc(1, config.net_input_buffer.size);
    config.net_input_buffer.position = 0;

    // Test: empty buffer returns error.
    config.net_input_buffer.position = 0;
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_ERROR, "Empty buffer returns error")

    // Test: simple GET request.
    mock_request(&config, "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse simple GET")
    ATT_ASSERT(config.request_method, WRD_REQUEST_GET, "Method is GET")
    ATT_ASSERT(strcmp(config.request_path, "/"), 0, "Path is /")
    ATT_ASSERT(config.page, WRD_PAGE_ROOT, "Page is root")
    ATT_ASSERT(config.http_version_major, 1, "HTTP major version")
    ATT_ASSERT(config.http_version_minor, 1, "HTTP minor version")
    ATT_ASSERT(config.request_headers_count, 1, "One header parsed")

    // Test: POST request.
    mock_request(&config, "POST /app/entries HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/json\r\nContent-Length: 13\r\n\r\n{\"key\":\"val\"}");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse POST")
    ATT_ASSERT(config.request_method, WRD_REQUEST_POST, "Method is POST")
    ATT_ASSERT(strcmp(config.request_path, "/app/entries"), 0, "Path is /app/entries")
    ATT_ASSERT(config.page, WRD_PAGE_APP, "Page is app")
    ATT_ASSERT(config.request_headers_count, 3, "Three headers parsed")

    // Test: GET with query string.
    mock_request(&config, "GET /app/entries?limit=5&offset=10 HTTP/1.1\r\nHost: localhost\r\n\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse GET with query string")
    ATT_ASSERT(strcmp(config.request_path, "/app/entries"), 0, "Path without query string")
    ATT_ASSERT(strcmp(config.request_query_string, "limit=5&offset=10"), 0, "Query string parsed")
    ATT_ASSERT(config.page, WRD_PAGE_APP, "Page is app with query")

    // Test: HEAD request.
    mock_request(&config, "HEAD / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse HEAD")
    ATT_ASSERT(config.request_method, WRD_REQUEST_HEAD, "Method is HEAD")

    // Test: PUT request.
    mock_request(&config, "PUT /app/entries HTTP/1.1\r\n\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse PUT")
    ATT_ASSERT(config.request_method, WRD_REQUEST_PUT, "Method is PUT")

    // Test: DELETE request.
    mock_request(&config, "DELETE /app/entries HTTP/1.1\r\n\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse DELETE")
    ATT_ASSERT(config.request_method, WRD_REQUEST_DELETE, "Method is DELETE")

    // Test: OPTIONS request.
    mock_request(&config, "OPTIONS /app/entries HTTP/1.1\r\n\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse OPTIONS")
    ATT_ASSERT(config.request_method, WRD_REQUEST_OPTIONS, "Method is OPTIONS")

    // Test: unknown method.
    mock_request(&config, "PATCH /app/entries HTTP/1.1\r\n\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse unknown method")
    ATT_ASSERT(config.request_method, WRD_REQUEST_UNKNOWN, "Unknown method")

    // Test: multiple headers.
    mock_request(&config,
        "GET / HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Accept: text/html\r\n"
        "User-Agent: Warudo-Test\r\n"
        "Connection: keep-alive\r\n"
        "\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse multiple headers")
    ATT_ASSERT(config.request_headers_count, 4, "Four headers parsed")

    // Test: wrd_http_get_header (case-insensitive).
    char *header_value = NULL;
    ATT_ASSERT(wrd_http_get_header(&config, "Host", &header_value), WRD_OK, "Get Host header")
    ATT_ASSERT(strcmp(header_value, "example.com"), 0, "Host value correct")

    ATT_ASSERT(wrd_http_get_header(&config, "host", &header_value), WRD_OK, "Get host (lowercase)")
    ATT_ASSERT(strcmp(header_value, "example.com"), 0, "Host value case-insensitive")

    ATT_ASSERT(wrd_http_get_header(&config, "user-agent", &header_value), WRD_OK, "Get User-Agent")
    ATT_ASSERT(strcmp(header_value, "Warudo-Test"), 0, "User-Agent value correct")

    ATT_ASSERT(wrd_http_get_header(&config, "X-Missing", &header_value), WRD_HTTP_NOT_FOUND, "Missing header returns not found")

    // Test: body offset.
    mock_request(&config,
        "POST / HTTP/1.1\r\n"
        "Content-Length: 5\r\n"
        "\r\n"
        "hello");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse with body")
    ATT_ASSERT(config.request_body_offset > 0, 1, "Body offset is set")
    ATT_ASSERT(strcmp(config.net_input_buffer.buffer + config.request_body_offset, "hello"), 0, "Body content accessible")

    // Test: /app/keys route.
    mock_request(&config, "GET /app/keys HTTP/1.1\r\n\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse /app/keys")
    ATT_ASSERT(config.page, WRD_PAGE_APP_KEYS, "Page is app keys")

    // Test: /app/dashboards route.
    mock_request(&config, "GET /app/dashboards HTTP/1.1\r\n\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse /app/dashboards")
    ATT_ASSERT(config.page, WRD_PAGE_APP_VIEWS, "Page is app views")

    // Test: unknown path.
    mock_request(&config, "GET /unknown HTTP/1.1\r\n\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_OK, "Parse unknown path")
    ATT_ASSERT(config.page, WRD_PAGE_NOT_FOUND, "Unknown path is not found")

    // Test: malformed request (no space).
    mock_request(&config, "GETHTTP/1.1\r\n\r\n");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_ERROR, "Malformed request line")

    // Test: malformed request (no \r\n).
    mock_request(&config, "GET / HTTP/1.1");
    ATT_ASSERT(wrd_http_parse_query_headers(&config), WRD_ERROR, "No CRLF in request")

    free(config.net_input_buffer.buffer);

    return NULL;
}
