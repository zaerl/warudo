#include <stdio.h>
#include <string.h>

#include "code.h"
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

wrd_code wrd_init(const char *filename, warudo **config) {
    CHECK_CONFIG

    *config = malloc(sizeof(warudo));
    int res;

    (*config)->columns_count = 0;
    (*config)->requests_count = 0;

#ifdef WRD_TIMING
    (*config)->timing_count = 0;
    (*config)->timing_end_count = 0;
#endif

    // Load net
    res = wrd_net_init(*config, wrd_get_env_int("WRD_LISTEN_BACKLOG", WRD_LISTEN_BACKLOG));

    if(res != WRD_OK) {
        wrd_close(*config);

        return res;
    }

    (*config)->page = -1;
    (*config)->request_method = WRD_REQUEST_UNKNOWN;
    (*config)->script_name = NULL;
    (*config)->query_string = NULL;

    // Query string
    wrd_parse_query_string(*config, NULL);

    // Environment variables
    (*config)->access_origin = wrd_get_env_string("WRD_CORS", WRD_DEFAULT_CORS);
    (*config)->log_level = wrd_get_env_int("WRD_LOG_LEVEL", WRD_DEFAULT_LOG_LEVEL);

    wrd_log_info(*config, "Starting warudo %s\n", WRD_VERSION);
    wrd_log_info(*config, "Access origin: %s\n", (*config)->access_origin ? (*config)->access_origin : "disabled");
    wrd_log_info(*config, "Log level: %u\n", (*config)->log_level);

    wrd_log_info(*config, "Net buffer size: %u bytes\n", (*config)->net_buffer.size);
    wrd_log_info(*config, "Net input buffer size: %u bytes\n", (*config)->net_buffer.size);
    wrd_log_info(*config, "Net headers size: %u bytes\n", (*config)->net_headers_buffer.size);
    wrd_log_info(*config, "Loading DB: \"%s\"\n", filename);

    // Load database
    res = wrd_db_init(filename, *config);

    if(res != WRD_OK) {
        wrd_close(*config);

        return WRD_DB_INIT_ERROR;
    }

    return WRD_OK;
}

wrd_code wrd_accept_connection(warudo *config) {
    CHECK_CONFIG

    wrd_code accepted = wrd_net_accept(config);

    if(accepted != WRD_OK) {
        wrd_log_error(config, "Failed to accept request %llu. Code %s\n",
                config->requests_count, wrd_error_description(accepted, 0));

        return accepted;
    }

#ifdef WRD_TIMING
    wrd_start_time(config);
#endif

    config->page = -1;
    config->request_method = WRD_REQUEST_UNKNOWN;
    config->script_name = NULL;
    config->query_string = NULL;
    ++config->requests_count;

    wrd_net_read(config);

    return WRD_OK;
}

wrd_code wrd_after_connection(warudo *config) {
    CHECK_CONFIG

    wrd_http_flush(config);
    wrd_net_finish_request(config);

#ifdef WRD_TIMING
    wrd_end_time(config, "after finish request");
#endif

    return WRD_OK;
}

wrd_code wrd_close(warudo *config) {
    CHECK_CONFIG

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
