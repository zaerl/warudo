#ifndef WRD_TIME_H
#define WRD_TIME_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

// Timing functions
wrd_code wrd_start_time(warudo *config);
wrd_code wrd_end_time(warudo *config, const char *message);

#ifdef __cplusplus
}
#endif

#endif // WRD_TIME_H