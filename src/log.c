#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "log.h"

WRD_API wrd_code wrd_log(warudo *config, wrd_log_level level, const char *format, ...) {
    if(format == NULL) {
        return WRD_ERROR;
    }

    wrd_log_level log_level = config ? config->log_level : WRD_DEFAULT_LOG_LEVEL;

    if(log_level == WRD_LOG_LEVEL_NO_LOG || level > log_level) {
        return WRD_OK;
    }

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);

    if(log_level == WRD_LOG_LEVEL_DEBUG && errno != 0) {
        strerror(errno);
    }

    return WRD_OK;
}
