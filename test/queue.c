#include <unistd.h>

#include "../src/queue.h"
#include "test.h"

void *test_queue(void *arg) {
    long worker_processes;
    wrd_code ret;
    MOCK_CONFIG

    ATT_ASSERT(wrd_get_workers(NULL, NULL), WRD_ERROR, "NULL NULL")
    ATT_ASSERT(wrd_get_workers(&config, NULL), WRD_ERROR, "NULL")
    ATT_ASSERT(wrd_get_workers(&config, &worker_processes), WRD_INVALID_CONFIG, "Invalid config")

    config.worker_processes = "0";
    ret = wrd_get_workers(&config, &worker_processes);
    ATT_ASSERT(ret, WRD_INVALID_CONFIG, "0")
    ATT_ASSERT(worker_processes, 0, "0 cores")

    config.worker_processes = "auto";
    ret = wrd_get_workers(&config, &worker_processes);
    ATT_ASSERT(ret, WRD_OK, "Auto")
    ATT_ASSERT(worker_processes, sysconf(_SC_NPROCESSORS_ONLN), "Auto cores")

    config.worker_processes = "1";
    ret = wrd_get_workers(&config, &worker_processes);
    ATT_ASSERT(ret, WRD_OK, "1")
    ATT_ASSERT(worker_processes, 1, "1 core")

    return NULL;
}
