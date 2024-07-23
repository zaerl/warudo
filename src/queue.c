#include <sys/errno.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/epoll.h>
#else
#include <sys/event.h>
#endif

#include "log.h"
#include "queue.h"

WRD_API wrd_code wrd_start_queue(warudo *config) {
    // Start the event queue.
    long num_workers;
    wrd_code code = wrd_get_workers(config, &num_workers);

    if(code == WRD_INVALID_CONFIG) {
        wrd_log_info(config, "Invalid number of workers: '%s'. Set to default 1.\n", config->workers);
        num_workers = 1;
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_get_workers(warudo *config, long *workers) {
    CHECK_CONFIG

    if(!workers) {
        return WRD_ERROR;
    }

    // The 'auto' value is used to automatically determine the number of workers.
    if(config && config->workers && strncasecmp(config->workers, "auto", 4) == 0) {
        *workers = sysconf(_SC_NPROCESSORS_ONLN);

        return WRD_OK;
    }

    if(!config->workers) {
        return WRD_INVALID_CONFIG;
    }

    char *endptr;
    errno = 0;
    long val = strtol(config->workers, &endptr, 10);

    if((errno == 0) && (*endptr == '\0') && (config->workers != endptr) && val > 0) {
        *workers = val;

        return WRD_OK;
    }

    return WRD_INVALID_CONFIG;
}
