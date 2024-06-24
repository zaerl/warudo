#include "code.h"

WRD_API const char *wrd_error_description(wrd_code code, int extended) {
    switch(code) {
        // Success code
        case WRD_OK:
            return extended ? "Success" : "OK";

        // HTTP codes
        case WRD_HTTP_BAD_REQUEST:
            return extended ? "404 Bad Request" : "BAD_REQUEST";
        case WRD_HTTP_NOT_FOUND:
            return extended ? "404 Not Found" : "WRD_HTTP_NOT_FOUND";
        case WRD_HTTP_NOT_ALLOWED:
            return extended ? "405 Not Allowed" : "WRD_HTTP_NOT_ALLOWED";
        case WRD_HTTP_INTERNAL_ERROR:
            return extended ? "500 Internal Server Error" : "WRD_HTTP_INTERNAL_ERROR";

        // Error codes
        case WRD_ERROR:
            return extended ? "Generic error" : "ERROR";
        case WRD_ADD_ERROR:
            return extended ? "Error while adding a new entry" : "ADD_ERROR";
        case WRD_DB_CLOSE_ERROR:
            return extended ? "Error while closing the database" : "DB_CLOSE_ERROR";
        case WRD_DB_ERROR:
            return extended ? "Error while accessing the database" : "DB_ERROR";
        case WRD_DB_INIT_ERROR:
            return extended ? "Error while initializing the database" : "DB_INIT_ERROR";
        case WRD_DB_OPEN_ERROR:
            return extended ? "Error while opening the database" : "DB_OPEN_ERROR";
        case WRD_EMPTY_CONTENT_ERROR:
            return extended ? "Empty content" : "EMPTY_CONTENT_ERROR";
        case WRD_EMPTY_QUERY_STRING_ERROR:
            return extended ? "Empty query string" : "EMPTY_QUERY_STRING_ERROR";
        case WRD_INIT_REQUEST_ERROR:
            return extended ? "Error while initializing the request" : "INIT_REQUEST_ERROR";
        case WRD_READ_ERROR:
            return extended ? "Error while reading from the socket" : "READ_ERROR";

        // Parser error codes
        case WRD_PARSER_EMPTY:
            return extended ? "Empty request" : "EMPTY_REQUEST";
        case WRD_PARSER_EMPTY_BOUNDARY:
            return extended ? "Empty boundary" : "EMPTY_BOUNDARY";
        case WRD_PARSER_VOID:
            return extended ? "Void request" : "PARSER_VOID";
        case WRD_PARSER_NO_BOUNDARY:
            return extended ? "No boundary" : "NO_BOUNDARY";
        case WRD_PARSER_MISSING_END:
            return extended ? "Missing end" : "MISSING_END";
        case WRD_PARSER_MISSING_CONTENT:
            return extended ? "Missing content" : "MISSING_CONTENT";
        case WRD_PARSER_MISSING_BODY:
            return extended ? "Missing body" : "MISSING_BODY";

        // Network error codes
        case WRD_SOCKET_ERROR:
            return extended ? "Error while creating socket" : "SOCKET_ERROR";
        case WRD_BIND_ERROR:
            return extended ? "Error while binding socket" : "BIND_ERROR";
        case WRD_LISTEN_ERROR:
            return extended ? "Error while listening on socket" : "LISTEN_ERROR";
        case WRD_ACCEPT_ERROR:
            return extended ? "Error while accepting connection" : "ACCEPT_ERROR";
        case WRD_CLOSE_ERROR:
            return extended ? "Error while closing socket" : "CLOSE_ERROR";

        case WRD_UNKNOWN_ERROR:
            return extended ? "Unknown error" : "UNKNOWN_ERROR";

    }

    return NULL;
}
