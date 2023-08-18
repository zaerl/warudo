#include <stdio.h>
#include <stdarg.h>

#include "warudo.h"

wrd_code wrd_log(warudo *config, int level, const char *format, ...) {
    CHECK_CONFIG

    if(format == NULL) {
        return WRD_ERROR;
    }

    if(config->log_level == WRD_LOG_LEVEL_NO_LOG || level > config->log_level) {
        return WRD_OK;
    }

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    return WRD_OK;
}
