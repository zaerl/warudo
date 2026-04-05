/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>

#include "warudo.h"

/*
#ifdef __linux__
#include <sys/epoll.h>
#else
#include <sys/event.h>
#endif
*/

WRD_API wrd_code wrd_worker_init(warudo *config) {
    // Start the workers.
    long num_workers;
    wrd_code code = wrd_get_workers(config, &num_workers);

    if(code == WRD_INVALID_CONFIG) {
        wrd_log_info(config, "Invalid number of worker processes: '%s'. Set to default 1.\n",
            config->worker_processes);
        num_workers = 1;
    }

    config->num_workers = num_workers;

    for(int i = 0; i < num_workers; ++i) {
        // Flush before fork to prevent child from duplicating buffered output.
        fflush(stdout);
        fflush(stderr);

        pid_t pid = fork();

        if(pid < 0) {
            wrd_log_error(config, u8"Failed to fork worker process. Code %s\n",
                strerror(errno));

            return WRD_ERROR;
        } else if(pid == 0) {
            // Child process.
            config->is_worker = 1;

            return WRD_OK;
        }


        // Allocate memory for the workers.
        if(!config->workers) {
            config->workers = malloc(num_workers * sizeof(wrd_worker));
        }

        // Parent process, add worker.
        config->workers[i].pid = pid;
        config->workers[i].status = WRD_LOADED;
        wrd_log_info(config, "Worker process %d started with PID %d\n", i, pid);
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_get_workers(warudo *config, long *workers) {
    CHECK_CONFIG

    if(!workers) {
        return WRD_ERROR;
    }

    // The 'auto' value is used to automatically determine the number of workers.
    if(config->worker_processes && strncasecmp(config->worker_processes, "auto", 4) == 0) {
        *workers = sysconf(_SC_NPROCESSORS_ONLN);

        return WRD_OK;
    }

    if(!config->worker_processes) {
        return WRD_INVALID_CONFIG;
    }

    char *endptr;
    errno = 0;
    long val = strtol(config->worker_processes, &endptr, 10);

    if((errno == 0) && (*endptr == '\0') && (config->worker_processes != endptr) && val > 0) {
        *workers = val;

        return WRD_OK;
    }

    return WRD_INVALID_CONFIG;
}

WRD_API wrd_code wrd_worker_close(warudo *config) {
    if(!config->is_worker && config->workers) {
        for(long i = 0; i < config->num_workers; ++i) {
            if(config->workers[i].pid > 0) {
                kill(config->workers[i].pid, SIGTERM);
            }
        }

        free(config->workers);
    }

    return WRD_OK;
}

// Determine if the connection should be kept alive based on HTTP version
// and Connection header. HTTP/1.1 defaults to keep-alive, HTTP/1.0 does not.
static int wrd_should_keep_alive(warudo *config) {
    if(config->keep_alive_timeout <= 0) {
        return 0;
    }

    char *conn_header = NULL;
    wrd_http_get_header(config, "Connection", &conn_header);

    if(conn_header) {
        if(strcasecmp(conn_header, "close") == 0) {
            return 0;
        }

        if(strcasecmp(conn_header, "keep-alive") == 0) {
            return 1;
        }
    }

    // HTTP/1.1 defaults to keep-alive, HTTP/1.0 does not.
    return config->http_version_major == 1 && config->http_version_minor >= 1;
}

WRD_API wrd_code wrd_worker_loop(warudo *config) {
    CHECK_CONFIG

    // Ignore SIGPIPE so broken connections return errors instead of killing
    // the worker process. Essential for TLS where mbedtls writes to sockets.
    signal(SIGPIPE, SIG_IGN);

    while(wrd_net_accept(config) == WRD_OK) {
        // Perform TLS handshake if TLS is enabled.
        if(config->tls_enabled && config->tls_state) {
            if(wrd_tls_handshake(config) != WRD_OK) {
                wrd_net_finish_request(config);
                continue;
            }
        }

        int keep_alive = 1;
        int request_count = 0;

        while(keep_alive) {
            if(wrd_net_read(config) != WRD_OK) {
                break;
            }

            // Timeout or client closed connection.
            if(config->net_input_buffer.position == 0) {
                break;
            }

            wrd_http_parse_query_headers(config);
            wrd_parse_query_string(config, config->request_query_string);

            ++config->requests_count;
            ++request_count;
            wrd_log_info(config, u8"Accepted request %llu\n", config->requests_count);

            keep_alive = wrd_should_keep_alive(config)
                && request_count < config->keep_alive_max;

            switch(config->page) {
                case WRD_PAGE_ROOT:
                    wrd_route_home(config);
                    break;
                case WRD_PAGE_APP:
                    wrd_route_app(WRD_ENTRY_TYPE_DATA, config);
                    break;
                case WRD_PAGE_APP_KEYS:
                    wrd_route_app_keys(config);
                    break;
                case WRD_PAGE_APP_VIEWS:
                    wrd_route_app(WRD_ENTRY_TYPE_VIEW, config);
                    break;
                default:
                    wrd_route_static(config);
                    break;
            }

            // Add Connection header after route handler has written status/headers.
            if(keep_alive) {
                wrd_http_buffer_puts(config, &config->net_headers_buffer,
                    "Connection: keep-alive\r\n");
            } else {
                wrd_http_buffer_puts(config, &config->net_headers_buffer,
                    "Connection: close\r\n");
            }

            wrd_http_flush(config);
        }

        wrd_net_finish_request(config);
    }

    return WRD_OK;
}
