#include <stdio.h>
#include <stdarg.h>

#include "warudo.h"

int warudo_log(warudo *config, int level, const char *format, ...) {
    CHECK_CONFIG

    if(format == NULL) {
        return WARUDO_ERROR;
    }

    if(config->log_level == WARUDO_LOG_LEVEL_NO_LOG || level > config->log_level) {
        return WARUDO_OK;
    }

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    return WARUDO_OK;
}
