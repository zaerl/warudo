#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "env.h"
#include "log.h"
#include "net.h"

wrd_code wrd_net_init(warudo *config, int backlog) {
    CHECK_CONFIG

    config->net_headers_buffer.buffer = NULL;
    config->net_buffer.buffer = NULL;
    config->net_input_buffer.buffer = NULL;

    config->net_headers_buffer.position = 0;
    config->net_buffer.position = 0;
    config->net_input_buffer.position = 0;

    // HTTP headers memory.
    config->net_headers_buffer.size = wrd_get_env_int("WRD_NET_HEADERS_BUFFER_SIZE", WRD_NET_HEADERS_BUFFER_SIZE);
    config->net_buffer.size = wrd_get_env_int("WRD_NET_BUFFER_SIZE", WRD_NET_BUFFER_SIZE);
    config->net_input_buffer.size = wrd_get_env_int("WRD_NET_INPUT_BUFFER_SIZE", WRD_NET_INPUT_BUFFER_SIZE);

    // config->net_buffer.size
    config->net_buffer.size *= 1048576;
    config->net_input_buffer.size *= 1048576;

    config->net_headers_buffer.buffer = calloc(1, config->net_headers_buffer.size);
    config->net_buffer.buffer = calloc(1, config->net_buffer.size);
    config->net_input_buffer.buffer = calloc(1, config->net_input_buffer.size);

    memset(config->net_headers_buffer.buffer, 0, config->net_headers_buffer.size);
    memset(config->net_buffer.buffer, 0, config->net_buffer.size);
    memset(config->net_input_buffer.buffer, 0, config->net_input_buffer.size);

    int res = WRD_OK;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Create a socket
    if(server_fd == -1) {
        return WRD_SOCKET_ERROR;
    }

    config->server_fd = server_fd;

    // Set up the address struct
    config->address.sin_family = AF_INET;
    config->address.sin_addr.s_addr = INADDR_ANY;
    config->address.sin_port = htons(WRD_SOCKET_PORT);

    // Bind the socket
    if(bind(config->server_fd, (struct sockaddr*)&config->address, sizeof(config->address)) < 0) {
        return WRD_BIND_ERROR;
    }

    if (listen(config->server_fd, backlog) < 0) {
        return WRD_LISTEN_ERROR;
    }

    return res == 0 ? WRD_OK : WRD_SOCKET_ERROR;
}

wrd_code wrd_net_close(warudo *config) {
    CHECK_CONFIG

    int res = WRD_OK;

    if(config->server_fd > 0) {
        if(close(config->server_fd) < 0) {
            res = WRD_CLOSE_ERROR;
        }
    }

    res = wrd_net_finish_request(config);

    return res;
}

wrd_code wrd_net_accept(warudo *config) {
    CHECK_CONFIG

    int addrlen = sizeof(config->address);
    int client_fd = accept(config->server_fd, (struct sockaddr *)&config->address, (socklen_t*)&addrlen);

    if (client_fd < 0) {
        config->client_fd = 0;

        return WRD_ACCEPT_ERROR;
    } else {
        // TODO: close after all listen
        config->client_fd = client_fd;
    }

    return WRD_OK;
}

wrd_code wrd_net_finish_request(warudo *config) {
    CHECK_CONFIG

    if(config->client_fd > 0) {
        if(close(config->client_fd) < 0) {
            return WRD_CLOSE_ERROR;
        }
    }

    return WRD_OK;
}

wrd_code wrd_net_read(warudo *config) {
    CHECK_CONFIG

    ssize_t res = read(config->client_fd, config->net_input_buffer.buffer, config->net_input_buffer.size);
    config->net_input_buffer.position = res;

    return WRD_OK;
}

wrd_code wrd_net_send(warudo *config, wrd_buffer *buffer) {
    CHECK_CONFIG

    if(buffer->position) {
        ssize_t sent = send(config->client_fd, buffer->buffer, buffer->position, 0);

        memset(buffer->buffer, 0, buffer->size);
        buffer->position = 0;

        return sent == -1 ? WRD_ERROR : WRD_OK;
    }

    return WRD_OK;
}
