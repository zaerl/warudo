
#include <ctype.h>

#include "conf.h"
#include "env.h"

// This file automatically generated. Do not edit it manually.

// Init a configuration file with environment variables.
void wrd_init_config(wrd_config *config) {
    // Database
    config->db_path = wrd_get_env_string("WRD_DB_PATH", WRD_DEFAULT_DB_PATH);
    // Log level can be one of the following [0, 1, 2, 3]: no_log, info, error, debug
    config->log_level = wrd_get_env_int("WRD_LOG_LEVEL", WRD_DEFAULT_LOG_LEVEL);
    // Net
    config->access_origin = wrd_get_env_string("WRD_ACCESS_ORIGIN", WRD_DEFAULT_ACCESS_ORIGIN);
    config->listen_backlog = wrd_get_env_int("WRD_LISTEN_BACKLOG", WRD_DEFAULT_LISTEN_BACKLOG);
    config->max_columns = wrd_get_env_int("WRD_MAX_COLUMNS", WRD_DEFAULT_MAX_COLUMNS);
    config->net_buffer_size = wrd_get_env_int("WRD_NET_BUFFER_SIZE", WRD_DEFAULT_NET_BUFFER_SIZE);
    config->net_headers_buffer_size = wrd_get_env_int("WRD_NET_HEADERS_BUFFER_SIZE", WRD_DEFAULT_NET_HEADERS_BUFFER_SIZE);
    config->net_input_buffer_size = wrd_get_env_int("WRD_NET_INPUT_BUFFER_SIZE", WRD_DEFAULT_NET_INPUT_BUFFER_SIZE);
    config->socket_port = wrd_get_env_int("WRD_SOCKET_PORT", WRD_DEFAULT_SOCKET_PORT);
    config->timing = wrd_get_env_int("WRD_TIMING", WRD_DEFAULT_TIMING);
}

// Load a configuration file.
ssize_t wrd_load_config(wrd_config *config, const char *file_path) {
    wrd_init_config(config);

    if(file_path == NULL) {
        return 0;
    }

    FILE *stream = fopen(file_path, "r");

    if(stream == NULL) {
        return 0;
    }

    char *line = NULL;
    size_t length = 0;
    ssize_t nread = 0;
    /*enum statuses {
        INIT,
        NAME,
        EQUALS,
        VALUE,
    } status = INIT;*/

    unsigned int loaded = 0;

    #define is_char(c) isalpha(c) || isdigit(c) || c == '.' || c == '-' || c == '_'

    while((nread = getline(&line, &length, stream)) != -1) {
        // printf(">>%s<<%zd", line, nread);
        size_t start_name = 0;
        size_t start_value = 0;

        // Empty line
        if(nread <= 1 || (nread == 1 && line[0] == '\n')) {
            continue;
        }

        // Comment
        if(line[0] == '#') {
            continue;
        }

        for(ssize_t i = 0; i < nread; ++i) {
            if(line[i] == '=') {
                start_value = i + 1;
            }

            /*if(status == INIT) {
                if(line[i] == ' ' || line[i] == '\t') {
                    continue;
                } else if(line[i] == '#') {
                    break;
                } else if(is_char(line[i])) {
                    status = NAME;
                    start_name = i;
                }
            } else if(status == NAME) {
                if(is_char(line[i])) {
                    continue;
                } else if (line[i] == ' ' || line[i] == '\t') {
                    status = EQUALS;
                } else if (line[i] == '=') {
                    status = VALUE;
                } else {
                    break;
                }
            } else if(status == EQUALS) {
                if(line[i] == ' ' || line[i] == '\t') {
                    continue;
                } else if(line[i] == '=') {
                    status = VALUE;
                } else {
                    break;
                }
            } else if(status == VALUE) {
                if(line[i] == ' ' || line[i] == '\t') {
                    continue;
                } else {
                    start_value = i;
                }
            }*/
        }

        /*if(status == VALUE && start_value > 0) {
            ++loaded;
        }*/
    }

    free(line);
    fclose(stream);

    return loaded;
}
