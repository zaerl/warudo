#include <stdio.h>
#include <stdarg.h>

#include "warudo.h"

int warudo_log(warudo *config, int level, const char *format, ...) {
    CHECK_CONFIG

    if(level > config->log_level) {
        return 0;
    }

    va_list args;
    va_start(args, format);
    int ret = vfprintf(stderr, format, args);
    va_end(args);

    return ret;
}
