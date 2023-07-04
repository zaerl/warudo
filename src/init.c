#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcgiapp.h>

#include "config.h"
#include "zaerl.h"
#include "db.h"

int zaerl_init(const char *filename, zaerl **config) {
    zaerl *pdb;
    int res;

    pdb = malloc(sizeof(zaerl));
    pdb->columns_count = 0;

    // Load database
    res = zaerl_db_init(filename, pdb);

    if(res != 0) {
        zaerl_close(pdb);

        return 1;
    }

    // Load FCGI
    res = FCGX_Init();

    if(res != 0) {
        zaerl_close(pdb);

        return 1;
    }

    // Create a socket to listen for connections
    int socket = FCGX_OpenSocket(ZAERL_SOCKET_PATH, 10);

    if(socket == -1) {
        zaerl_close(pdb);

        return 1;
    }

    pdb->socket = socket;

    // Initialize the request structure
    FCGX_InitRequest(&pdb->request, pdb->socket, 0);

    if(res != 0) {
        zaerl_close(pdb);

        return 1;
    }

    pdb->page = -1;
    pdb->request_method = ZAERL_REQUEST_UNKNOWN;
    pdb->script_name = NULL;
    pdb->query_string = NULL;

    *config = pdb;

    return 0;
}

int zaerl_accept_connection(zaerl *config) {
    config->page = -1;
    config->request_method = ZAERL_REQUEST_UNKNOWN;
    config->script_name = NULL;
    config->query_string = NULL;
    const char* request_method = NULL;

    int accepted = FCGX_Accept_r(&config->request);

    if(accepted < 0) {
        return accepted;
    }

    char** envp = config->request.envp;

    for(; *envp != NULL; envp++) {
        // FCGX_FPrintF(config->request.out, "%s\n", *envp);
        if(strncmp(*envp, "SCRIPT_NAME=", 12) == 0) {
            config->script_name = *envp + 12;
        } else if(strncmp(*envp, "QUERY_STRING=", 13) == 0) {
            config->query_string = *envp + 13;
        } else if(strncmp(*envp, "REQUEST_METHOD=", 15) == 0) {
            request_method = *envp + 15;
        }
    }

    if(config->script_name != NULL && strcmp(config->script_name, "/") == 0) {
        config->page = ZAERL_PAGE_ROOT;
    } else if(config->script_name != NULL && strcmp(config->script_name, "/app") == 0) {
        config->page = ZAERL_PAGE_APP;
    } else {
        config->page = ZAERL_PAGE_NOT_FOUND;
    }

    if(request_method != NULL) {
        if(strcmp(request_method, "GET") == 0) {
            config->request_method = ZAERL_REQUEST_GET;
        } else if(strcmp(request_method, "POST") == 0) {
            config->request_method = ZAERL_REQUEST_POST;
        } else {
            config->request_method = ZAERL_REQUEST_UNKNOWN;
        }
    }

    return accepted;
}

int zaerl_close(zaerl *config) {
    if(!config) {
        return 0;
    }

    FCGX_Free(&config->request, 1);
    zaerl_db_close(config);

    free(config);

    return 0;
}
