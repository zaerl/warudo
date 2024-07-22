#ifndef WRD_CONF_H
#define WRD_CONF_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WRD_DEFAULT_CONFIG = 'D',
    WRD_LOADED_CONFIG = 'C',
    WRD_ENV_CONFIG = 'E',
    WRD_NOT_LOADED_CONFIG = 'N',
} wrd_config_status;

// Init configurations with default values.
WRD_API wrd_code wrd_init_config(warudo *config);

// Set configurations to environment variable values, if they exist.
WRD_API wrd_code wrd_load_config_env(warudo *config);

// Close loaded configurations.
WRD_API wrd_code wrd_config_close(warudo *config);

// Load a configuration file.
WRD_API wrd_code wrd_load_config(warudo *config, const char *file_path);

// Get the status of a configuration.
WRD_API wrd_config_status wrd_get_config_status(warudo *config, wrd_config_name name);

// Get the status of a configuration.
WRD_API wrd_config_status wrd_get_config_status(warudo *config, wrd_config_name name);

// Set the status of a configuration.
WRD_API wrd_code wrd_set_config_status(warudo *config, wrd_config_name name, wrd_config_status status);

#ifdef __cplusplus
}
#endif

#endif // WRD_CONF_H
