#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "code.h"
#include "conf.h"
#include "db.h"
#include "env.h"
#include "log.h"
#include "net.h"
#include "query.h"
#include "worker.h"
#include "server.h"
#include "timing.h"

WRD_API wrd_code wrd_server_init(warudo *config) {
    if(!config) {
        return WRD_ERROR;
    }

    wrd_code ret;

    char *conf_file = NULL;
    wrd_get_env_string(&conf_file, "WRD_CONF_PATH");

    // load warudo.conf file.
    ret = wrd_config_init(config, conf_file ? conf_file : WRD_DEFAULT_CONF_PATH);

    if(conf_file) {
        free(conf_file);
    }

    if(!(ret == WRD_OK || ret == WRD_DEFAULT)) {
        wrd_server_close(config);

        return ret;
    }

    wrd_log_info(config, u8"Starting Warudo %s\n", WRD_VERSION);

    if(ret != WRD_OK) {
        wrd_server_close(config);

        return ret;
    }

    // Configurations.
    wrd_log_info(config, u8"Db Path: %s [%c]\n", config->db_path, wrd_get_config_status(config, WRD_DB_PATH));
    wrd_log_info(config, u8"Log Level: %d [%c]\n", config->log_level, wrd_get_config_status(config, WRD_LOG_LEVEL));
    wrd_log_info(config, u8"Access Origin: %s [%c]\n", config->access_origin, wrd_get_config_status(config, WRD_ACCESS_ORIGIN));
    wrd_log_info(config, u8"Listen Backlog: %d [%c]\n", config->listen_backlog, wrd_get_config_status(config, WRD_LISTEN_BACKLOG));
    wrd_log_info(config, u8"Max Columns: %d [%c]\n", config->max_columns, wrd_get_config_status(config, WRD_MAX_COLUMNS));
    wrd_log_info(config, u8"Net Buffer Size: %d [%c]\n", config->net_buffer_size, wrd_get_config_status(config, WRD_NET_BUFFER_SIZE));
    wrd_log_info(config, u8"Net Headers Buffer Size: %d [%c]\n", config->net_headers_buffer_size, wrd_get_config_status(config, WRD_NET_HEADERS_BUFFER_SIZE));
    wrd_log_info(config, u8"Net Input Buffer Size: %d [%c]\n", config->net_input_buffer_size, wrd_get_config_status(config, WRD_NET_INPUT_BUFFER_SIZE));
    wrd_log_info(config, u8"Socket Port: %d [%c]\n", config->socket_port, wrd_get_config_status(config, WRD_SOCKET_PORT));
    wrd_log_info(config, u8"Timing: %d [%c]\n", config->timing, wrd_get_config_status(config, WRD_TIMING));
    wrd_log_info(config, u8"Worker Processes: %s [%c]\n", config->worker_processes, wrd_get_config_status(config, WRD_WORKER_PROCESSES));

    ret = wrd_db_init(config->db_path, config);

    if(ret != WRD_OK) {
        wrd_server_close(config);

        return WRD_DB_INIT_ERROR;
    }

    // Start all the workers.
    ret = wrd_worker_init(config);

    if(ret != WRD_OK) {
        wrd_server_close(config);

        return ret;
    }

    // Current process is a worker.
    if(config->is_worker) {
        // Load net
        /*ret = wrd_net_init(config, config->listen_backlog);

        if(ret != WRD_OK) {
            wrd_server_close(config);

            return ret;
        }

        config->page = -1;
        config->request_method = WRD_REQUEST_UNKNOWN;
        config->script_name = NULL;
        config->query_string = NULL;

        // Query string
        wrd_parse_query_string(config, NULL);

        while(wrd_accept_connection(config) == WRD_OK) {
            wrd_http_parse_query_headers(config);
            wrd_log_info(config, u8"Accepted request %llu\n", config->requests_count);

            wrd_http_ok(config);
            wrd_http_puts(config, u8"{\"Hello\": \"World\"}");

            wrd_after_connection(config);
        }*/

        return WRD_OK;
    } else {
        wrd_log_info(config, u8"Master process\n", NULL);
        wrd_server_loop(config);
    }

    // Master process.
    /*while(true) {
        int status;

        pid_i exit_pid = wait(&status);

        if(exit_pid < 0) {
            wrd_log_error(config, u8"Failed to wait for worker process. Code %s\n", strerror(errno));

            return WRD_ERROR;
        }

        wrd_log_info(config, u8"Worker process %d exited with status %d\n", exit_pid, status);

        pid_t pid = fork();

        if(pid < 0) {
            wrd_log_error(config, u8"Failed to fork worker process. Code %s\n", strerror(errno));

            return WRD_ERROR;
        } else if(pid == 0) {
            // Child process.
            // config->is_worker = 1;

            // return WRD_OK;
        }
    }*/


    return WRD_OK;
}

WRD_API wrd_code wrd_server_loop(warudo *config) {
    CHECK_CONFIG

    if(config->is_worker) {
        // This is the parent loop.
        return WRD_ERROR;
    }

    while(1) {
        int status;
        pid_t exit_pid = wait(&status);

        if(exit_pid < 0) {
            wrd_log_error(config, u8"Failed to wait for worker process. Code %s\n", strerror(errno));

            return WRD_ERROR;
        }

        wrd_log_info(config, u8"Worker process %d exited with status %d\n", exit_pid, status);
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_accept_connection(warudo *config) {
    CHECK_CONFIG

    wrd_code accepted = wrd_net_accept(config);

    if(accepted != WRD_OK) {
        wrd_log_error(config, u8"Failed to accept request %llu. Code %s\n",
                config->requests_count, wrd_error_description(accepted, 0));

        return accepted;
    }

    if(config->timing) {
        wrd_start_time(config);
    }

    config->page = -1;
    config->request_method = WRD_REQUEST_UNKNOWN;
    config->script_name = NULL;
    config->query_string = NULL;
    ++config->requests_count;

    wrd_net_read(config);

    return WRD_OK;
}

WRD_API wrd_code wrd_after_connection(warudo *config) {
    CHECK_CONFIG

    // wrd_http_flush(config);
    wrd_net_finish_request(config);

    if(config->timing) {
        wrd_end_time(config, u8"after finish request");
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_server_close(warudo *config) {
    CHECK_CONFIG

    wrd_config_close(config);
    wrd_worker_close(config);
    wrd_net_close(config);
    wrd_db_close(config);

    if(config->net_headers_buffer.buffer != NULL) {
        free(config->net_headers_buffer.buffer);
    }

    if(config->net_buffer.buffer != NULL) {
        free(config->net_buffer.buffer);
    }

    if(config->net_input_buffer.buffer != NULL) {
        free(config->net_input_buffer.buffer);
    }

    free(config);

    return WRD_OK;
}
