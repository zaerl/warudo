#include "code.h"

WRD_API const char *wrd_error_description(wrd_code code, int extended) {
    switch(code) {
        // Success codes
        case WRD_OK:
            return extended ? u8"Success" : u8"OK";
        case WRD_DEFAULT:
            return extended ? u8"Default" : u8"DEFAULT";
        case WRD_LOADED:
            return extended ? u8"Loaded config" : u8"LOADED";

        // HTTP codes
        case WRD_HTTP_BAD_REQUEST:
            return extended ? u8"404 Bad Request" : u8"BAD_REQUEST";
        case WRD_HTTP_NOT_FOUND:
            return extended ? u8"404 Not Found" : u8"WRD_HTTP_NOT_FOUND";
        case WRD_HTTP_NOT_ALLOWED:
            return extended ? u8"405 Not Allowed" : u8"WRD_HTTP_NOT_ALLOWED";
        case WRD_HTTP_INTERNAL_ERROR:
            return extended ? u8"500 Internal Server Error" : u8"WRD_HTTP_INTERNAL_ERROR";

        // Error codes
        case WRD_ERROR:
            return extended ? u8"Generic error" : u8"ERROR";
        case WRD_ADD_ERROR:
            return extended ? u8"Error while adding a new entry" : u8"ADD_ERROR";
        case WRD_DB_CLOSE_ERROR:
            return extended ? u8"Error while closing the database" : u8"DB_CLOSE_ERROR";
        case WRD_DB_ERROR:
            return extended ? u8"Error while accessing the database" : u8"DB_ERROR";
        case WRD_DB_INIT_ERROR:
            return extended ? u8"Error while initializing the database" : u8"DB_INIT_ERROR";
        case WRD_DB_OPEN_ERROR:
            return extended ? u8"Error while opening the database" : u8"DB_OPEN_ERROR";
        case WRD_EMPTY_CONTENT_ERROR:
            return extended ? u8"Empty content" : u8"EMPTY_CONTENT_ERROR";
        case WRD_EMPTY_QUERY_STRING_ERROR:
            return extended ? u8"Empty query string" : u8"EMPTY_QUERY_STRING_ERROR";
        case WRD_INIT_REQUEST_ERROR:
            return extended ? u8"Error while initializing the request" : u8"INIT_REQUEST_ERROR";
        case WRD_READ_ERROR:
            return extended ? u8"Error while reading from the socket" : u8"READ_ERROR";
        case WRD_FILE_ERROR:
            return extended ? u8"Error while opening the file" : u8"FILE_ERROR";
        case WRD_MEMORY_ERROR:
            return extended ? u8"Memory allocation error" : u8"MEMORY_ERROR";
        case WRD_MISSING_CRYPTO_ERROR:
            return extended ? u8"Missing crypto error" : u8"MISSING_CRYPTO_ERROR";
        case WRD_INVALID_CONFIG:
            return extended ? u8"Invalid configuration" : u8"INVALID_CONFIG";

        // Parser error codes
        case WRD_PARSER_EMPTY:
            return extended ? u8"Empty request" : u8"EMPTY_REQUEST";
        case WRD_PARSER_EMPTY_BOUNDARY:
            return extended ? u8"Empty boundary" : u8"EMPTY_BOUNDARY";
        case WRD_PARSER_VOID:
            return extended ? u8"Void request" : u8"PARSER_VOID";
        case WRD_PARSER_NO_BOUNDARY:
            return extended ? u8"No boundary" : u8"NO_BOUNDARY";
        case WRD_PARSER_MISSING_END:
            return extended ? u8"Missing end" : u8"MISSING_END";
        case WRD_PARSER_MISSING_CONTENT:
            return extended ? u8"Missing content" : u8"MISSING_CONTENT";
        case WRD_PARSER_MISSING_BODY:
            return extended ? u8"Missing body" : u8"MISSING_BODY";

        // Network error codes
        case WRD_SOCKET_ERROR:
            return extended ? u8"Error while creating socket" : u8"SOCKET_ERROR";
        case WRD_BIND_ERROR:
            return extended ? u8"Error while binding socket" : u8"BIND_ERROR";
        case WRD_LISTEN_ERROR:
            return extended ? u8"Error while listening on socket" : u8"LISTEN_ERROR";
        case WRD_ACCEPT_ERROR:
            return extended ? u8"Error while accepting connection" : u8"ACCEPT_ERROR";
        case WRD_CLOSE_ERROR:
            return extended ? u8"Error while closing socket" : u8"CLOSE_ERROR";
        case WRD_SETSOCKOPT_ERROR:
            return extended ? u8"Error while setting socket options" : u8"SETSOCKOPT_ERROR";

        case WRD_UNKNOWN_ERROR:
            return extended ? u8"Unknown error" : u8"UNKNOWN_ERROR";

    }

    return NULL;
}
