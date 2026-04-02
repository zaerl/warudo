#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "code.h"
#include "conf.h"
#include "db.h"
#include "env.h"
#include "fs.h"
#include "log.h"
#include "net.h"
#include "query.h"
#include "routes.h"
#include "server.h"
#include "timing.h"
#include "worker.h"

static volatile sig_atomic_t wrd_shutdown = 0;

static void wrd_signal_handler(int sig) {
    (void)sig;
    wrd_shutdown = 1;
}

WRD_API wrd_code wrd_server_init(warudo *config) {
    if(!config) {
        return WRD_ERROR;
    }

    wrd_code ret;

    char *conf_file = NULL;
    wrd_get_env_string(&conf_file, "WRD_CONF_PATH");

    // Load configuration file.
    ret = wrd_config_init(config, conf_file ? conf_file : WRD_DEFAULT_CONF_PATH);

    if(conf_file) {
        free(conf_file);
    }

    if(!(ret == WRD_OK || ret == WRD_DEFAULT)) {
        wrd_server_close(config);

        return ret;
    }

    ret = wrd_server_save_pid(config);

    if(ret != WRD_OK) {
        wrd_server_close(config);

        return ret;
    }

    wrd_log_info(config, u8"Starting Warudo %s\n", WRD_VERSION);

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
    wrd_log_info(config, u8"Pid File: %s [%c]\n", config->pid_file, wrd_get_config_status(config, WRD_PID_FILE));
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
        ret = wrd_net_init(config, config->listen_backlog);

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

        wrd_worker_loop(config);

        return WRD_OK;
    } else {
        struct sigaction sa;
        sa.sa_handler = wrd_signal_handler;
        sa.sa_flags = 0; // No SA_RESTART, so wait() returns EINTR.

        sigemptyset(&sa.sa_mask);
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGINT, &sa, NULL);
        wrd_log_info(config, u8"Master process\n", NULL);
        wrd_server_loop(config);
    }

    return WRD_OK;
}

// Save PID file.
WRD_API wrd_code wrd_server_save_pid(warudo *config) {
    CHECK_CONFIG

    if(config->pid_file == NULL) {
        // Nothing to load, return.
        return WRD_ERROR;
    }

    int fd;

    // Try to open the file and ensure it is created only if it doesn't exist.
    fd = open(config->pid_file, O_WRONLY | O_CREAT | O_EXCL, 0644);

    if(fd == -1) {
        if(errno == EEXIST) {
            // Check if the process from the existing PID file is still running.
            FILE *f = fopen(config->pid_file, "r");

            if(f) {
                int old_pid = 0;

                fscanf(f, "%d", &old_pid);
                fclose(f);

                if(old_pid > 0 && kill(old_pid, 0) == 0) {
                    // Process is still running.
                    wrd_log_error(config, u8"Another instance is running with PID %d\n", old_pid);

                    return WRD_ERROR;
                }
            }

            // Stale PID file, remove and retry.
            unlink(config->pid_file);
            fd = open(config->pid_file, O_WRONLY | O_CREAT | O_EXCL, 0644);

            if(fd == -1) {
                wrd_log_error(config, u8"Failed to create PID file: %s. Error: %s\n",
                    config->pid_file, strerror(errno));

                return WRD_ERROR;
            }
        } else {
            wrd_log_error(config, u8"Failed to open PID file: %s. Error: %s\n",
                config->pid_file, strerror(errno));

            return WRD_ERROR;
        }
    }

    // Get the current process ID.
    pid_t pid = getpid();

    // Write the PID to the file.
    dprintf(fd, "%d\n", pid);

    // Close the file.
    close(fd);

    return WRD_OK;
}

WRD_API wrd_code wrd_server_loop(warudo *config) {
    CHECK_CONFIG

    if(config->is_worker) {
        // This is the parent loop.
        return WRD_ERROR;
    }

    while(!wrd_shutdown) {
        int status;
        pid_t exit_pid = wait(&status);

        if(exit_pid < 0) {
            if(errno == EINTR) {
                // Interrupted by signal, check shutdown flag.
                continue;
            }

            if(errno == ECHILD) {
                wrd_log_info(config, u8"All worker processes exited\n", NULL);
            } else {
                wrd_log_error(config, u8"Failed to wait for worker process. Code %s\n", strerror(errno));
            }

            break;
        }

        if(WIFEXITED(status)) {
            wrd_log_info(config, u8"Worker process %d exited with status %d\n", exit_pid, WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)) {
            wrd_log_info(config, u8"Worker process %d killed by signal %d\n", exit_pid, WTERMSIG(status));
        }
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_server_close(warudo *config) {
    CHECK_CONFIG

    // Remove PID file on shutdown.
    if(config->pid_file && !config->is_worker) {
        unlink(config->pid_file);
    }

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

/*WRD_API wrd_code wrd_accept_connection(warudo *config) {
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
*/
