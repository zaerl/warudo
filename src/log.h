#ifndef WRD_LOG_H
#define WRD_LOG_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

wrd_code wrd_log(warudo *config, wrd_log_level level, const char *format, ...);

#define wrd_log_error(config, format, ...) wrd_log(config, WRD_LOG_LEVEL_ERROR, format, __VA_ARGS__);
#define wrd_log_info(config, format, ...) wrd_log(config, WRD_LOG_LEVEL_INFO, format, __VA_ARGS__);
#define wrd_log_debug(config, format, ...) wrd_log(config, WRD_LOG_LEVEL_DEBUG, format, __VA_ARGS__);

#ifdef __cplusplus
}
#endif

#endif // WRD_LOG_H
