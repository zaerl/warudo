#ifndef WRD_CONF_H
#define WRD_CONF_H

// This file automatically generated. Do not edit it manually.

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

// Load a configuration file.
WRD_API void wrd_init_config(warudo *config);

// Load a configuration file.
WRD_API wrd_code wrd_load_config(warudo *config, const char *file_path);

#ifdef __cplusplus
}
#endif

#endif // WRD_CONF_H
