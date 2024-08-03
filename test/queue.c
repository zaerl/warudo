#include <unistd.h>

#include "../src/queue.h"
#include "test.h"

void *test_queue(void *arg) {
    long worker_processes = 0;
    long default_workers = sysconf(_SC_NPROCESSORS_ONLN);
    wrd_code ret = WRD_OK;
    MOCK_CONFIG

    ATT_ASSERT(wrd_get_workers(NULL, NULL), WRD_ERROR, "NULL NULL workers")
    ATT_ASSERT(wrd_get_workers(&config, NULL), WRD_ERROR, "NULL workers")
    ATT_ASSERT(wrd_get_workers(&config, &worker_processes), WRD_OK, "Invalid config")
    ATT_ASSERT(worker_processes, default_workers, "System cores")

    config.worker_processes = "0";
    worker_processes = 0;
    ret = wrd_get_workers(&config, &worker_processes);
    ATT_ASSERT(ret, WRD_INVALID_CONFIG, "0 workers")
    ATT_ASSERT(worker_processes, 0, "0 cores")

    config.worker_processes = "auto";
    ret = wrd_get_workers(&config, &worker_processes);
    ATT_ASSERT(ret, WRD_OK, "Auto processes")
    ATT_ASSERT(worker_processes, sysconf(_SC_NPROCESSORS_ONLN), "Auto cores")

    config.worker_processes = "1";
    ret = wrd_get_workers(&config, &worker_processes);
    ATT_ASSERT(ret, WRD_OK, "1 process")
    ATT_ASSERT(worker_processes, 1, "1 core")

    return NULL;
}
