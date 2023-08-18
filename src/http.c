#include "http.h"

#define WARUDO_CHECK_CONNECTION(request) \
    if(request.in == NULL || \
        request.out == NULL || \
        request.err == NULL) { \
        return WARUDO_FCGI_INIT_ERROR; \
    }

warudo_code warudo_status(const char* status, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    FCGX_FPrintF(config->request.out, "Status: %s\r\n", status);

    return WARUDO_OK;
}

warudo_code warudo_content_type(const char* content_type, warudo *config) {
    WARUDO_CHECK_CONNECTION(config->request);

    if(config->access_origin != NULL) {
        FCGX_FPrintF(config->request.out, "Access-Control-Allow-Origin: %s\r\n", config->access_origin);
    }

    FCGX_FPrintF(config->request.out, "Content-type: %s;charset=utf-8\r\n\r\n", content_type);

    return WARUDO_OK;
}

warudo_code warudo_header(const char* status, const char* content_type, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_status(status, config);
    warudo_content_type(content_type, config);

    return WARUDO_OK;
}

warudo_code warudo_ok(warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_header("200 OK", "application/json", config);

    return WARUDO_OK;
}

warudo_code warudo_created(unsigned long long int id, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_header("201 Created", "application/json", config);
    FCGX_FPrintF(config->request.out, "{\"status\":\"success\",\"id\":%lld}", id);

    return WARUDO_OK;
}

warudo_code warudo_multi_created(unsigned long int count, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_header("201 Created", "application/json", config);
    FCGX_FPrintF(config->request.out, "{\"status\":\"success\",\"count\":%lld}", count);

    return WARUDO_OK;
}

warudo_code warudo_not_allowed(const char* allowed, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_status("405 Method Not Allowed", config);
    FCGX_FPrintF(config->request.out, "Allow: %s\r\n", allowed);
    warudo_content_type("text/type", config);
    FCGX_PutS("Not allowed.", config->request.out);

    return WARUDO_HTTP_NOT_ALLOWED;
}

warudo_code warudo_server_error(const char* description, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_header("500 Internal Server Error", "text/plain", config);
    FCGX_PutS(description, config->request.out);

    return WARUDO_HTTP_INTERNAL_ERROR;
}

warudo_code warudo_bad_request(const char* description, warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_header("400 Bad Request", "application/json", config);
    FCGX_FPrintF(config->request.out, "{\"status\":\"failure\",\"error\":\"%s\"}", description);

    return WARUDO_HTTP_BAD_REQUEST;
}

warudo_code warudo_page_not_found(warudo* config) {
    WARUDO_CHECK_CONNECTION(config->request);

    warudo_header("404 Not Found", "text/plain", config);
    FCGX_PutS("Unknown.", config->request.out);

    return WARUDO_HTTP_NOT_FOUND;
}
