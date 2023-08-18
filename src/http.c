#include "http.h"

#define WRD_CHECK_CONNECTION(request) \
    if(request.in == NULL || \
        request.out == NULL || \
        request.err == NULL) { \
        return WRD_FCGI_INIT_ERROR; \
    }

wrd_code wrd_status(const char *status, warudo *config) {
    WRD_CHECK_CONNECTION(config->request);

    FCGX_FPrintF(config->request.out, "Status: %s\r\n", status);

    return WRD_OK;
}

wrd_code wrd_content_type(const char *content_type, warudo *config) {
    WRD_CHECK_CONNECTION(config->request);

    if(config->access_origin != NULL) {
        FCGX_FPrintF(config->request.out, "Access-Control-Allow-Origin: %s\r\n", config->access_origin);
    }

    FCGX_FPrintF(config->request.out, "Content-type: %s;charset=utf-8\r\n\r\n", content_type);

    return WRD_OK;
}

wrd_code wrd_header(const char *status, const char *content_type, warudo *config) {
    WRD_CHECK_CONNECTION(config->request);

    wrd_status(status, config);
    wrd_content_type(content_type, config);

    return WRD_OK;
}

wrd_code wrd_ok(warudo *config) {
    WRD_CHECK_CONNECTION(config->request);

    wrd_header("200 OK", "application/json", config);

    return WRD_OK;
}

wrd_code wrd_created(unsigned long long int id, warudo *config) {
    WRD_CHECK_CONNECTION(config->request);

    wrd_header("201 Created", "application/json", config);
    FCGX_FPrintF(config->request.out, "{\"status\":\"success\",\"id\":%lld}", id);

    return WRD_OK;
}

wrd_code wrd_multi_created(unsigned long int count, warudo *config) {
    WRD_CHECK_CONNECTION(config->request);

    wrd_header("201 Created", "application/json", config);
    FCGX_FPrintF(config->request.out, "{\"status\":\"success\",\"count\":%lld}", count);

    return WRD_OK;
}

wrd_code wrd_not_allowed(const char *allowed, warudo *config) {
    WRD_CHECK_CONNECTION(config->request);

    wrd_status("405 Method Not Allowed", config);
    FCGX_FPrintF(config->request.out, "Allow: %s\r\n", allowed);
    wrd_content_type("text/type", config);
    FCGX_PutS("Not allowed.", config->request.out);

    return WRD_HTTP_NOT_ALLOWED;
}

wrd_code wrd_server_error(const char *description, warudo *config) {
    WRD_CHECK_CONNECTION(config->request);

    wrd_header("500 Internal Server Error", "text/plain", config);
    FCGX_PutS(description, config->request.out);

    return WRD_HTTP_INTERNAL_ERROR;
}

wrd_code wrd_bad_request(const char *description, warudo *config) {
    WRD_CHECK_CONNECTION(config->request);

    wrd_header("400 Bad Request", "application/json", config);
    FCGX_FPrintF(config->request.out, "{\"status\":\"failure\",\"error\":\"%s\"}", description);

    return WRD_HTTP_BAD_REQUEST;
}

wrd_code wrd_not_found(warudo *config) {
    WRD_CHECK_CONNECTION(config->request);

    wrd_header("404 Not Found", "text/plain", config);
    FCGX_PutS("Unknown.", config->request.out);

    return WRD_HTTP_NOT_FOUND;
}
