#ifndef WRD_TLS_H
#define WRD_TLS_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

// Init TLS with default values.
WRD_API wrd_code wrd_init_tls(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WRD_TLS_H
