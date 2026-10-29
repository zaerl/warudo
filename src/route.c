/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <string.h>

#include "warudo.h"

// The path is not copied, it must outlive the server (usually a string literal).
WRD_API wrd_code wrd_route_add(warudo *config, const char *path, wrd_route_handler handler) {
    CHECK_CONFIG

    if(path == NULL || handler == NULL) {
        return WRD_ERROR;
    }

    // Replace the handler of an already registered path.
    for(unsigned int i = 0; i < config->routes_count; ++i) {
        if(strcmp(config->routes[i].path, path) == 0) {
            config->routes[i].handler = handler;

            return WRD_OK;
        }
    }

    if(config->routes_count >= WRD_MAX_ROUTES) {
        return WRD_ERROR;
    }

    config->routes[config->routes_count].path = path;
    config->routes[config->routes_count].handler = handler;
    ++config->routes_count;

    return WRD_OK;
}

WRD_API wrd_route_handler wrd_route_find(warudo *config, const char *path) {
    if(!config || config->status != WRD_LOADED || path == NULL) {
        return NULL;
    }

    for(unsigned int i = 0; i < config->routes_count; ++i) {
        if(strcmp(config->routes[i].path, path) == 0) {
            return config->routes[i].handler;
        }
    }

    return NULL;
}

// Run the handler registered for the request path, or fall back to static files.
WRD_API wrd_code wrd_route_dispatch(warudo *config) {
    CHECK_CONFIG

    wrd_route_handler handler = wrd_route_find(config, config->request_path);

    if(handler != NULL) {
        return handler(config);
    }

    return wrd_route_static(config);
}
