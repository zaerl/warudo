#ifndef WARUDO_LOG_H
#define WARUDO_LOG_H

#include "warudo.h"

int warudo_log(warudo *config, int level, const char *format, ...);

#define warudo_log_error(config, format, ...) warudo_log(config, WARUDO_LOG_LEVEL_ERROR, format, __VA_ARGS__);
#define warudo_log_info(config, format, ...) warudo_log(config, WARUDO_LOG_LEVEL_INFO, format, __VA_ARGS__);
#define warudo_log_debug(config, format, ...) warudo_log(config, WARUDO_LOG_LEVEL_DEBUG, format, __VA_ARGS__);

#endif /* WARUDO_LOG_H */
