#include <unistd.h>

#include "../src/queue.h"
#include "test.h"

void *test_queue(void *arg) {
    long workers;
    wrd_code ret;
    MOCK_CONFIG

    ATT_ASSERT(wrd_get_workers(NULL, NULL), WRD_ERROR, "NULL NULL")
    ATT_ASSERT(wrd_get_workers(&config, NULL), WRD_ERROR, "NULL")
    ATT_ASSERT(wrd_get_workers(&config, &workers), WRD_INVALID_CONFIG, "Invalid config")

    config.workers = "0";
    ret = wrd_get_workers(&config, &workers);
    ATT_ASSERT(ret, WRD_INVALID_CONFIG, "0")
    ATT_ASSERT(workers, 0, "0 cores")

    config.workers = "auto";
    ret = wrd_get_workers(&config, &workers);
    ATT_ASSERT(ret, WRD_OK, "Auto")
    ATT_ASSERT(workers, sysconf(_SC_NPROCESSORS_ONLN), "Auto cores")

    config.workers = "1";
    ret = wrd_get_workers(&config, &workers);
    ATT_ASSERT(ret, WRD_OK, "1")
    ATT_ASSERT(workers, 1, "1 core")

    return NULL;
}
