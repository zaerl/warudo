#include <stdio.h>
#include <string.h>

#include "code.h"
#include "conf.h"
#include "db.h"
#include "env.h"
#include "log.h"
#include "net.h"
#include "query.h"
#include "server.h"
#include "timing.h"

// TODO: remove
#include "http.h"

// TODO: remove
#include <unistd.h>

WRD_API wrd_code wrd_init_server(warudo *config) {
    CHECK_CONFIG

    int res;

    char *conf_file = NULL;
    wrd_get_env_string(&conf_file, "WRD_CONF_PATH");

    res = wrd_load_config(config, conf_file ? conf_file : WRD_DEFAULT_CONF_PATH);

    if(conf_file) {
        free(conf_file);
    }

    if(!(res == WRD_OK || res == WRD_DEFAULT)) {
        wrd_close(config);

        return res;
    }

    // Load net
    // res = wrd_net_init(config, wrd_get_env_int("WRD_LISTEN_BACKLOG", WRD_LISTEN_BACKLOG));
    res = wrd_net_init(config, config->listen_backlog);

    if(res != WRD_OK) {
        wrd_close(config);

        return res;
    }

    config->page = -1;
    config->request_method = WRD_REQUEST_UNKNOWN;
    config->script_name = NULL;
    config->query_string = NULL;

    // Query string
    wrd_parse_query_string(config, NULL);

    wrd_log_info(config, u8"Starting Warudo %s\n", WRD_VERSION);

    // Configurations.
    wrd_log_info(config, u8"Db Path: %s [%c]\n", config->db_path, wrd_get_config_status(config, WRD_DB_PATH));
    wrd_log_info(config, u8"Log Level: %d [%c]\n", config->log_level, wrd_get_config_status(config, WRD_LOG_LEVEL));
    wrd_log_info(config, u8"Access Origin: %s [%c]\n", config->access_origin, wrd_get_config_status(config, WRD_ACCESS_ORIGIN));
    wrd_log_info(config, u8"Listen Backlog: %d [%c]\n", config->listen_backlog, wrd_get_config_status(config, WRD_LISTEN_BACKLOG));
    wrd_log_info(config, u8"Max Columns: %d [%c]\n", config->max_columns, wrd_get_config_status(config, WRD_MAX_COLUMNS));
    wrd_log_info(config, u8"Net Buffer Size: %d [%c]\n", config->net_buffer_size, wrd_get_config_status(config, WRD_NET_BUFFER_SIZE));
    wrd_log_info(config, u8"Net Headers Buffer Size: %d [%c]\n", config->net_headers_buffer_size, wrd_get_config_status(config, WRD_NET_HEADERS_BUFFER_SIZE));
    wrd_log_info(config, u8"Net Input Buffer Size: %d [%c]\n", config->net_input_buffer_size, wrd_get_config_status(config, WRD_NET_INPUT_BUFFER_SIZE));
    wrd_log_info(config, u8"Socket Port: %d [%c]\n", config->socket_port, wrd_get_config_status(config, WRD_SOCKET_PORT));
    wrd_log_info(config, u8"Timing: %d [%c]\n", config->timing, wrd_get_config_status(config, WRD_TIMING));

    res = wrd_db_init(config->db_path, config);

    if(res != WRD_OK) {
        wrd_close(config);

        return WRD_DB_INIT_ERROR;
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_accept_connection(warudo *config) {
    CHECK_CONFIG

    wrd_code accepted = wrd_net_accept(config);

    if(accepted != WRD_OK) {
        wrd_log_error(config, u8"Failed to accept request %llu. Code %s\n",
                config->requests_count, wrd_error_description(accepted, 0));

        return accepted;
    }

    if(config->timing) {
        wrd_start_time(config);
    }

    config->page = -1;
    config->request_method = WRD_REQUEST_UNKNOWN;
    config->script_name = NULL;
    config->query_string = NULL;
    ++config->requests_count;

    wrd_net_read(config);

    return WRD_OK;
}

WRD_API wrd_code wrd_after_connection(warudo *config) {
    CHECK_CONFIG

    wrd_http_flush(config);
    wrd_net_finish_request(config);

    if(config->timing) {
        wrd_end_time(config, u8"after finish request");
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_close(warudo *config) {
    CHECK_CONFIG

    wrd_config_close(config);
    wrd_net_close(config);
    wrd_db_close(config);

    if(config->net_headers_buffer.buffer != NULL) {
        free(config->net_headers_buffer.buffer);
    }

    if(config->net_buffer.buffer != NULL) {
        free(config->net_buffer.buffer);
    }

    if(config->net_input_buffer.buffer != NULL) {
        free(config->net_input_buffer.buffer);
    }

    free(config);

    return WRD_OK;
}
