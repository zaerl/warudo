/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "warudo.h"

// Create, bind, and listen on a TCP socket for the given port.
static int wrd_create_socket(int port, struct sockaddr_in *address, int backlog) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if(fd == -1) {
        return -1;
    }

    int optval = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);

    if(bind(fd, (struct sockaddr*)address, sizeof(*address)) < 0) {
        close(fd);
        return -1;
    }

    if(listen(fd, backlog) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

WRD_API wrd_code wrd_net_init(warudo *config, int backlog) {
    CHECK_CONFIG

    config->net_headers_buffer.buffer = NULL;
    config->net_buffer.buffer = NULL;
    config->net_input_buffer.buffer = NULL;

    config->net_headers_buffer.position = 0;
    config->net_buffer.position = 0;
    config->net_input_buffer.position = 0;

    config->net_headers_buffer.size = config->net_headers_buffer_size;
    config->net_buffer.size = config->net_buffer_size;
    config->net_input_buffer.size = config->net_input_buffer_size;

    config->net_buffer.size *= 1048576;
    config->net_input_buffer.size *= 1048576;

    config->net_headers_buffer.buffer = calloc(1, config->net_headers_buffer.size);
    config->net_buffer.buffer = calloc(1, config->net_buffer.size);
    config->net_input_buffer.buffer = calloc(1, config->net_input_buffer.size);

    memset(config->net_headers_buffer.buffer, 0, config->net_headers_buffer.size);
    memset(config->net_buffer.buffer, 0, config->net_buffer.size);
    memset(config->net_input_buffer.buffer, 0, config->net_input_buffer.size);

    // When TLS is enabled with HSTS, bind two sockets: server_fd for HTTP (redirects) or
    // server_tls_fd for HTTPS. Otherwise, bind a single socket on port.
    if(config->tls_enabled && config->hsts_max_age > 0) {
        if(config->port == config->tls_port) {
            wrd_log_error(config,
                "port and tls_port cannot be the same (%d)%s\n",
                config->port, "");

            return WRD_INVALID_CONFIG;
        }
        config->server_tls_fd = wrd_create_socket(config->tls_port, &config->address, backlog);

        if(config->server_tls_fd == -1) {
            return WRD_SOCKET_ERROR;
        }

        struct sockaddr_in http_address;
        config->server_fd = wrd_create_socket(config->port, &http_address, backlog);

        if(config->server_fd == -1) {
            close(config->server_tls_fd);
            config->server_tls_fd = 0;

            return WRD_SOCKET_ERROR;
        }
    } else {
        int port = config->tls_enabled ? config->tls_port : config->port;
        config->server_fd = wrd_create_socket(port, &config->address, backlog);

        if(config->server_fd == -1) {
            return WRD_SOCKET_ERROR;
        }
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_net_close(warudo *config) {
    CHECK_CONFIG

    int res = WRD_OK;

    if(config->server_fd > 0) {
        if(close(config->server_fd) < 0) {
            res = WRD_CLOSE_ERROR;
        }
    }

    if(config->server_tls_fd > 0) {
        if(close(config->server_tls_fd) < 0) {
            res = WRD_CLOSE_ERROR;
        }
    }

    res = wrd_net_finish_request(config);

    return res;
}

WRD_API wrd_code wrd_net_accept(warudo *config) {
    CHECK_CONFIG

    int addrlen = sizeof(config->address);
    int ready_fd = config->server_fd;

    // When both HTTP and HTTPS sockets are active, use select() to accept from whichever socket has
    // a pending connection.
    if(config->server_tls_fd > 0) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(config->server_fd, &read_fds);
        FD_SET(config->server_tls_fd, &read_fds);

        int max_fd = config->server_fd > config->server_tls_fd
            ? config->server_fd : config->server_tls_fd;

        int sel = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if(sel < 0) {
            config->client_fd = 0;

            return WRD_ACCEPT_ERROR;
        }

        if(FD_ISSET(config->server_tls_fd, &read_fds)) {
            ready_fd = config->server_tls_fd;
            config->is_tls_connection = 1;
        } else {
            ready_fd = config->server_fd;
            config->is_tls_connection = 0;
        }
    } else {
        // Single socket mode: TLS if enabled, plain otherwise.
        config->is_tls_connection = config->tls_enabled;
    }

    int client_fd = accept(ready_fd, (struct sockaddr *)&config->address, (socklen_t*)&addrlen);

    if(client_fd < 0) {
        config->client_fd = 0;

        return WRD_ACCEPT_ERROR;
    }

    config->client_fd = client_fd;

    // Set receive timeout for keep-alive idle connections.
    if(config->keep_alive_timeout > 0) {
        struct timeval tv;
        tv.tv_sec = config->keep_alive_timeout;
        tv.tv_usec = 0;
        setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_net_finish_request(warudo *config) {
    CHECK_CONFIG

    // Close TLS session before closing the socket.
    if(config->tls_ssl) {
        wrd_tls_finish_request(config);
    }

    if(config->client_fd > 0) {
        if(close(config->client_fd) < 0) {
            return WRD_CLOSE_ERROR;
        }
    }

    return WRD_OK;
}

// Wait for activity on the client connection or server listening sockets. Returns WRD_OK if the
// client has data ready to read. Returns WRD_READ_ERROR if a new connection is pending on a server
// socket, or on timeout. In both cases the caller should break out of the keep-alive loop.
WRD_API wrd_code wrd_net_poll(warudo *config) {
    CHECK_CONFIG

    // If TLS has buffered decrypted data, skip select — data is already ready.
    if(config->tls_ssl && wrd_tls_pending(config)) {
        return WRD_OK;
    }

    fd_set read_fds;
    FD_ZERO(&read_fds);

    FD_SET(config->client_fd, &read_fds);
    int max_fd = config->client_fd;

    if(config->server_fd > 0) {
        FD_SET(config->server_fd, &read_fds);

        if(config->server_fd > max_fd) {
            max_fd = config->server_fd;
        }
    }

    if(config->server_tls_fd > 0) {
        FD_SET(config->server_tls_fd, &read_fds);

        if(config->server_tls_fd > max_fd) {
            max_fd = config->server_tls_fd;
        }
    }

    struct timeval tv;
    tv.tv_sec = config->keep_alive_timeout;
    tv.tv_usec = 0;

    int sel = select(max_fd + 1, &read_fds, NULL, NULL, &tv);

    if(sel <= 0) {
        return WRD_READ_ERROR;
    }

    // Client has data — continue keep-alive.
    if(FD_ISSET(config->client_fd, &read_fds)) {
        return WRD_OK;
    }

    // New connection pending on a server socket — break keep-alive.
    return WRD_READ_ERROR;
}

WRD_API int wrd_net_peek_byte(warudo *config) {
    if(!config || config->client_fd <= 0) {
        return -1;
    }

    unsigned char byte;
    ssize_t res = recv(config->client_fd, &byte, 1, MSG_PEEK);

    return res == 1 ? byte : -1;
}

WRD_API wrd_code wrd_net_read(warudo *config) {
    CHECK_CONFIG

    ssize_t res;

    if(config->tls_ssl) {
        res = wrd_tls_read(config, (unsigned char *)config->net_input_buffer.buffer,
            config->net_input_buffer.size);
    } else {
        res = read(config->client_fd, config->net_input_buffer.buffer,
            config->net_input_buffer.size);
    }

    if(res <= 0) {
        config->net_input_buffer.position = 0;

        return WRD_READ_ERROR;
    }

    config->net_input_buffer.position = res;

    return WRD_OK;
}

WRD_API wrd_code wrd_net_send(warudo *config, wrd_buffer *buffer) {
    CHECK_CONFIG

    if(buffer->position) {
        ssize_t sent;

        if(config->tls_ssl) {
            sent = wrd_tls_write(config, (const unsigned char *)buffer->buffer, buffer->position);
        } else {
            sent = send(config->client_fd, buffer->buffer, buffer->position, 0);
        }

        memset(buffer->buffer, 0, buffer->size);
        buffer->position = 0;

        return sent <= 0 ? WRD_ERROR : WRD_OK;
    }

    return WRD_OK;
}
