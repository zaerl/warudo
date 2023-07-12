#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcgiapp.h>

#include "config.h"
#include "warudo.h"
#include "db.h"

int warudo_init(const char *filename, warudo **config) {
    warudo *pdb;
    int res;

    pdb = malloc(sizeof(warudo));
    pdb->columns_count = 0;
    pdb->requests_count = 0;
    pdb->access_origin = WARUDO_CORS;

    // Load database
    res = warudo_db_init(filename, pdb);

    if(res != 0) {
        warudo_close(pdb);

        return 1;
    }

    // Load FCGI
    res = FCGX_Init();

    if(res != 0) {
        warudo_close(pdb);

        return 1;
    }

    // Create a socket to listen for connections
    int socket = FCGX_OpenSocket(WARUDO_SOCKET_PATH, 10);

    if(socket == -1) {
        warudo_close(pdb);

        return 1;
    }

    pdb->socket = socket;

    // Initialize the request structure
    FCGX_InitRequest(&pdb->request, pdb->socket, 0);

    if(res != 0) {
        warudo_close(pdb);

        return 1;
    }

    pdb->page = -1;
    pdb->request_method = WARUDO_REQUEST_UNKNOWN;
    pdb->script_name = NULL;
    pdb->query_string = NULL;

    // Query string
    pdb->query_id = 0;
    pdb->query_limit = 0;
    pdb->query_key = NULL;
    pdb->query_value = NULL;

    // Environment variables
    char* access_origin = getenv("WARUDO_CORS");

    if(access_origin != NULL) {
        printf("Access origin %s\n", access_origin);
        pdb->access_origin = access_origin;
    }

    *config = pdb;

    return 0;
}

int warudo_parse_query_string(char* query_string, warudo* config) {
    if(query_string == NULL) {
        return 1;
    }

    char* saveptr;
    char* parameter = strtok_r(query_string, "&", &saveptr);

    while(parameter != NULL) {
        char *delimiter = strchr(parameter, '=');

        if(delimiter != NULL) {
            int delimiter_idx = delimiter - parameter;
            int length_1 = delimiter_idx;
            // int length_2 = strlen(parameter) - delimiter_idx - 1;
            char* value = delimiter + 1;

            if(value != NULL) {
                ZA_GET_QUERY_INT_VALUE(parameter, limit, value, length_1)
                else ZA_GET_QUERY_INT_VALUE(parameter, id, value, length_1)
                else ZA_GET_QUERY_STRING_VALUE(parameter, key, value, length_1)
                else ZA_GET_QUERY_STRING_VALUE(parameter, value, value, length_1)
            }
        }

        parameter = strtok_r(NULL, "&", &saveptr);
    }

    return 0;
}

int warudo_accept_connection(warudo *config) {
    config->page = -1;
    config->request_method = WARUDO_REQUEST_UNKNOWN;
    config->script_name = NULL;
    config->query_string = NULL;
    ++config->requests_count;

    // Query string
    ZA_FREE_QUERY_INT_VALUE(id, 0)
    ZA_FREE_QUERY_INT_VALUE(limit, WARUDO_DEFAULT_QUERY_LIMIT)
    ZA_FREE_QUERY_STRING_VALUE(key)
    ZA_FREE_QUERY_STRING_VALUE(value)

    int accepted = FCGX_Accept_r(&config->request);

    if(accepted < 0) {
        return 1;
    }

    const char* script_name = FCGX_GetParam("SCRIPT_NAME", config->request.envp);
    const char* query_string = FCGX_GetParam("QUERY_STRING", config->request.envp);
    const char* request_method = FCGX_GetParam("REQUEST_METHOD", config->request.envp);

    if(script_name != NULL && strcmp(script_name, "/") == 0) {
        config->page = WARUDO_PAGE_ROOT;
    } else if(script_name != NULL && strcmp(script_name, "/app/entries") == 0) {
        config->page = WARUDO_PAGE_APP;
    } else if(script_name != NULL && strcmp(script_name, "/app/keys") == 0) {
        config->page = WARUDO_PAGE_APP_KEYS;
    } else if(script_name != NULL && strcmp(script_name, "/app/views") == 0) {
        config->page = WARUDO_PAGE_APP_VIEWS;
    } else {
        config->page = WARUDO_PAGE_NOT_FOUND;
    }

    if(request_method != NULL) {
        if(strcmp(request_method, "GET") == 0) {
            config->request_method = WARUDO_REQUEST_GET;
        } else if(strcmp(request_method, "POST") == 0) {
            config->request_method = WARUDO_REQUEST_POST;
        } else {
            config->request_method = WARUDO_REQUEST_UNKNOWN;
        }
    }

    if(query_string != NULL) {
        warudo_parse_query_string((char*)query_string, config);
    }

    config->script_name = script_name;
    config->query_string = query_string;

    return 0;
}

int warudo_close(warudo *config) {
    if(!config) {
        return 0;
    }

    FCGX_Free(&config->request, 1);
    warudo_db_close(config);

    free(config);

    return 0;
}
