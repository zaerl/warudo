#ifndef WRD_QUEUE_H
#define WRD_QUEUE_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

// Start the event queue.
WRD_API wrd_code wrd_queue_init(warudo *config);
WRD_API wrd_code wrd_get_workers(warudo *config, long *workers);
WRD_API wrd_code wrd_queue_close(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WRD_QUEUE_H