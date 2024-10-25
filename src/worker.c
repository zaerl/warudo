#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "log.h"
#include "worker.h"

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

    for(int i = 0; i < num_workers; ++i) {
        pid_t pid = fork();

        if(pid < 0) {
            wrd_log_error(config, u8"Failed to fork worker process. Code %s\n",
                strerror(errno));

            return WRD_ERROR;
        } else if(pid == 0) {
            // Child process.
            config->is_worker = 1;
            wrd_worker_loop(config);

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
    // Close the workers.
    if(config->workers) {
        free(config->workers);
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_worker_loop(warudo *config) {
    return WRD_OK;
}
