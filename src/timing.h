#ifndef WRD_TIME_H
#define WRD_TIME_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

// Timing functions
WRD_API wrd_code wrd_start_time(warudo *config);
WRD_API wrd_code wrd_end_time(warudo *config, const char *message);

#ifdef __cplusplus
}
#endif

#endif // WRD_TIME_H
