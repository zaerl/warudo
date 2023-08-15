#include "warudo.h"

const char* warudo_error_description(int code, int extended) {
    switch(code) {
        // Success code
        case WARUDO_OK:
            return extended ? "Success" : "OK";

        // Error codes
        case WARUDO_ERROR:
            return extended ? "Generic error" : "ERROR";
        case WARUDO_ACCEPT_ERROR:
            return extended ? "Error while accepting connection" : "ACCEPT_ERROR";
        case WARUDO_ADD_ERROR:
            return extended ? "Error while adding a new entry" : "ADD_ERROR";
        case WARUDO_DB_CLOSE_ERROR:
            return extended ? "Error while closing the database" : "DB_CLOSE_ERROR";
        case WARUDO_DB_ERROR:
            return extended ? "Error while accessing the database" : "DB_ERROR";
        case WARUDO_DB_INIT_ERROR:
            return extended ? "Error while initializing the database" : "DB_INIT_ERROR";
        case WARUDO_DB_OPEN_ERROR:
            return extended ? "Error while opening the database" : "DB_OPEN_ERROR";
        case WARUDO_EMPTY_CONTENT_ERROR:
            return extended ? "Empty content" : "EMPTY_CONTENT_ERROR";
        case WARUDO_EMPTY_QUERY_STRING_ERROR:
            return extended ? "Empty query string" : "EMPTY_QUERY_STRING_ERROR";
        case WARUDO_FCGI_INIT_ERROR:
            return extended ? "Error while initializing FastCGI" : "FCGI_INIT_ERROR";
        case WARUDO_INIT_REQUEST_ERROR:
            return extended ? "Error while initializing the request" : "INIT_REQUEST_ERROR";
        case WARUDO_READ_ERROR:
            return extended ? "Error while reading from the socket" : "READ_ERROR";
        case WARUDO_SOCKET_ERROR:
            return extended ? "Error while creating the socket" : "SOCKET_ERROR";

        // Parser error codes
        case WARUDO_PARSER_EMPTY:
            return extended ? "Empty request" : "EMPTY_REQUEST";
        case WARUDO_PARSER_EMPTY_BOUNDARY:
            return extended ? "Empty boundary" : "EMPTY_BOUNDARY";
        case WARUDO_PARSER_VOID:
            return extended ? "Void request" : "PARSER_VOID";
        case WARUDO_PARSER_NO_BOUNDARY:
            return extended ? "No boundary" : "NO_BOUNDARY";
        case WARUDO_PARSER_MISSING_END:
            return extended ? "Missing end" : "MISSING_END";
        case WARUDO_PARSER_MISSING_CONTENT:
            return extended ? "Missing content" : "MISSING_CONTENT";
        case WARUDO_PARSER_MISSING_BODY:
            return extended ? "Missing body" : "MISSING_BODY";
        case WARUDO_UNKNOWN_ERROR:
            return extended ? "Unknown error" : "UNKNOWN_ERROR";
    }

    return NULL;
}
