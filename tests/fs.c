#include "test.h"

void *test_fs(void *arg) {
    // MIME type tests.
    ATT_ASSERT(strcmp(wrd_mime_type(NULL), "application/octet-stream"), 0, "NULL path")
    ATT_ASSERT(strcmp(wrd_mime_type("noext"), "application/octet-stream"), 0, "No extension")
    ATT_ASSERT(strcmp(wrd_mime_type("file.unknown"), "application/octet-stream"), 0, "Unknown ext")
    ATT_ASSERT(strcmp(wrd_mime_type("index.html"), "text/html"), 0, "HTML")
    ATT_ASSERT(strcmp(wrd_mime_type("index.htm"), "text/html"), 0, "HTM")
    ATT_ASSERT(strcmp(wrd_mime_type("style.css"), "text/css"), 0, "CSS")
    ATT_ASSERT(strcmp(wrd_mime_type("app.js"), "application/javascript"), 0, "JS")
    ATT_ASSERT(strcmp(wrd_mime_type("data.json"), "application/json"), 0, "JSON")
    ATT_ASSERT(strcmp(wrd_mime_type("image.png"), "image/png"), 0, "PNG")
    ATT_ASSERT(strcmp(wrd_mime_type("photo.jpg"), "image/jpeg"), 0, "JPG")
    ATT_ASSERT(strcmp(wrd_mime_type("photo.jpeg"), "image/jpeg"), 0, "JPEG")
    ATT_ASSERT(strcmp(wrd_mime_type("icon.svg"), "image/svg+xml"), 0, "SVG")
    ATT_ASSERT(strcmp(wrd_mime_type("favicon.ico"), "image/x-icon"), 0, "ICO")
    ATT_ASSERT(strcmp(wrd_mime_type("doc.pdf"), "application/pdf"), 0, "PDF")
    ATT_ASSERT(strcmp(wrd_mime_type("font.woff2"), "font/woff2"), 0, "WOFF2")
    ATT_ASSERT(strcmp(wrd_mime_type("module.wasm"), "application/wasm"), 0, "WASM")

    // Case insensitive.
    ATT_ASSERT(strcmp(wrd_mime_type("FILE.HTML"), "text/html"), 0, "HTML uppercase")
    ATT_ASSERT(strcmp(wrd_mime_type("STYLE.CSS"), "text/css"), 0, "CSS uppercase")

    // Path with directories.
    ATT_ASSERT(strcmp(wrd_mime_type("/path/to/file.js"), "application/javascript"), 0, "JS with path")

    // wrd_route_static without document_root returns 404.
    MOCK_CONFIG

    config.net_headers_buffer.size = 4096;
    config.net_headers_buffer.buffer = calloc(1, config.net_headers_buffer.size);
    config.net_headers_buffer.position = 0;
    config.net_buffer.size = 4096;
    config.net_buffer.buffer = calloc(1, config.net_buffer.size);
    config.net_buffer.position = 0;
    config.client_fd = 1; // Fake fd so WRD_CHECK_CONNECTION passes.

    config.document_root = NULL;
    config.request_method = WRD_REQUEST_GET;
    strcpy(config.request_path, "/test.html");
    ATT_ASSERT(wrd_route_static(&config), WRD_HTTP_NOT_FOUND, "No document_root returns 404")

    // Path traversal rejected.
    config.document_root = "/tmp";
    strcpy(config.request_path, "/../etc/passwd");
    ATT_ASSERT(wrd_route_static(&config), WRD_HTTP_NOT_FOUND, "Path traversal rejected")

    // POST not allowed.
    config.request_method = WRD_REQUEST_POST;
    strcpy(config.request_path, "/test.html");
    ATT_ASSERT(wrd_route_static(&config), WRD_HTTP_NOT_ALLOWED, "POST not allowed")

    free(config.net_headers_buffer.buffer);
    free(config.net_buffer.buffer);

    return NULL;
}
