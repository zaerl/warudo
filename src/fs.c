/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "warudo.h"

// Read a file into a buffer.
WRD_API wrd_code wrd_read_file(const char *file_path, void **file_buffer,
    long *file_size, long max_size) {
    if(file_path == NULL) {
        // Nothing to load, return.
        return WRD_ERROR;
    }

    if(max_size == 0) {
        *file_buffer = NULL;

        return WRD_OK;
    }

    FILE *file = fopen(file_path, "r");

    if(!file) {
        return WRD_FILE_ERROR;
    }

    fseek(file, 0, SEEK_END);
    long fs_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if(fs_size == 0) {
        *file_size = 0;
        *file_buffer = NULL;

        return WRD_OK;
    }

    *file_size = fs_size;
    void *fs_buffer = malloc(fs_size);

    if(!fs_buffer) {
        fclose(file);

        return WRD_MEMORY_ERROR;
    }

    *file_size = fs_size;
    *file_buffer = fs_buffer;

    fread(*file_buffer, 1, *file_size, file);
    fclose(file);

    return WRD_OK;
}

// MIME type lookup by file extension.
typedef struct {
    const char *ext;
    const char *mime;
} wrd_mime_entry;

static const wrd_mime_entry wrd_mime_types[] = {
    { ".html", "text/html" },
    { ".htm",  "text/html" },
    { ".css",  "text/css" },
    { ".js",   "application/javascript" },
    { ".json", "application/json" },
    { ".xml",  "application/xml" },
    { ".txt",  "text/plain" },
    { ".csv",  "text/csv" },
    { ".svg",  "image/svg+xml" },
    { ".png",  "image/png" },
    { ".jpg",  "image/jpeg" },
    { ".jpeg", "image/jpeg" },
    { ".gif",  "image/gif" },
    { ".ico",  "image/x-icon" },
    { ".webp", "image/webp" },
    { ".avif", "image/avif" },
    { ".woff", "font/woff" },
    { ".woff2","font/woff2" },
    { ".ttf",  "font/ttf" },
    { ".otf",  "font/otf" },
    { ".pdf",  "application/pdf" },
    { ".zip",  "application/zip" },
    { ".wasm", "application/wasm" },
    { ".mp4",  "video/mp4" },
    { ".webm", "video/webm" },
    { ".mp3",  "audio/mpeg" },
    { ".ogg",  "audio/ogg" },
    { NULL, NULL }
};

WRD_API const char *wrd_mime_type(const char *path) {
    if(!path) {
        return "application/octet-stream";
    }

    const char *dot = strrchr(path, '.');

    if(!dot) {
        return "application/octet-stream";
    }

    for(int i = 0; wrd_mime_types[i].ext != NULL; ++i) {
        if(strcasecmp(dot, wrd_mime_types[i].ext) == 0) {
            return wrd_mime_types[i].mime;
        }
    }

    return "application/octet-stream";
}

// Check if a resolved path is safe (no path traversal outside document_root).
static int wrd_is_safe_path(const char *path) {
    if(strstr(path, "..") != NULL) {
        return 0;
    }

    return 1;
}

// Serve a static file from document_root.
WRD_API wrd_code wrd_route_static(warudo *config) {
    if(!config || config->status != WRD_LOADED) {
        return WRD_ERROR;
    }

    if(!config->document_root) {
        return wrd_http_not_found(config);
    }

    if(config->request_method != WRD_REQUEST_GET
        && config->request_method != WRD_REQUEST_HEAD) {
        return wrd_http_not_allowed(config, "GET, HEAD");
    }

    if(!wrd_is_safe_path(config->request_path)) {
        return wrd_http_not_found(config);
    }

    size_t root_len = strlen(config->document_root);
    size_t path_len = strlen(config->request_path);
    char *full_path = malloc(root_len + path_len + 12);

    if(!full_path) {
        return wrd_http_server_error(config, "Memory error");
    }

    memcpy(full_path, config->document_root, root_len);
    memcpy(full_path + root_len, config->request_path, path_len);
    full_path[root_len + path_len] = '\0';

    if(path_len > 0 && config->request_path[path_len - 1] == '/') {
        memcpy(full_path + root_len + path_len, "index.html", 11);
    }

    void *file_buffer = NULL;
    long file_size = 0;
    wrd_code ret = wrd_read_file(full_path, &file_buffer, &file_size, WRD_MAX_FILE_SIZE);

    if(ret != WRD_OK || !file_buffer) {
        free(full_path);

        if(file_buffer) {
            free(file_buffer);
        }

        return wrd_http_not_found(config);
    }

    const char *mime = wrd_mime_type(full_path);
    free(full_path);

    wrd_http_header(config, "200 OK", mime);

    if(config->request_method == WRD_REQUEST_GET) {
        char *buf = config->net_buffer.buffer + config->net_buffer.position;
        size_t available = config->net_buffer.size - config->net_buffer.position;

        if((size_t)file_size <= available) {
            memcpy(buf, file_buffer, file_size);
            config->net_buffer.position += file_size;
        }
    }

    free(file_buffer);

    return WRD_OK;
}
