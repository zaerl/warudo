#ifndef WRD_WORKER_H
#define WRD_WORKER_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

// Start the workers.
WRD_API wrd_code wrd_worker_init(warudo *config);
WRD_API wrd_code wrd_get_workers(warudo *config, long *workers);
WRD_API wrd_code wrd_worker_close(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WRD_WORKER_H
