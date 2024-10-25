#include "log.h"
#include "timing.h"

WRD_API wrd_code wrd_start_time(warudo *config) {
    CHECK_CONFIG

    ++config->timing_count;
    config->timing_end_count = 0;
    clock_gettime(CLOCK_MONOTONIC, &config->start);

    return WRD_OK;
}

WRD_API wrd_code wrd_end_time(warudo *config, const char *message) {
    CHECK_CONFIG

    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);

    ++config->timing_end_count;

    long long int elapsed = (end.tv_sec - config->start.tv_sec) +
        (end.tv_nsec - config->start.tv_nsec);

    wrd_log_info(config, u8"%llu: %llu time %llu ns [%s]\n", config->timing_count,
        config->timing_end_count, elapsed, message ? message : "");

    return WRD_OK;
}
