#include <stdio.h>
#include <string.h>

#include "warudo.h"

#include "db.h"
#include "env.h"
#include "fcgi.h"
#include "log.h"
#include "net.h"
#include "query.h"
#include "time.h"

// TODO: remove
#include "http.h"

// TODO: remove
#include <unistd.h>

wrd_code wrd_init(const char *filename, warudo **config) {
    CHECK_CONFIG

    warudo *pdb;
    int res;

    pdb = malloc(sizeof(warudo));
    pdb->columns_count = 0;
    pdb->requests_count = 0;

#ifdef WRD_TIMING
    pdb->timing_count = 0;
    pdb->timing_end_count = 0;
#endif

    // Load net
    res = wrd_net_init(pdb, wrd_get_env_int("WARUDO_LISTEN_BACKLOG", WRD_LISTEN_BACKLOG));

    if(res != WRD_OK) {
        wrd_close(pdb);

        return res;
    }

    pdb->page = -1;
    pdb->request_method = WRD_REQUEST_UNKNOWN;
    pdb->script_name = NULL;
    pdb->query_string = NULL;

    // Query string
    wrd_parse_query_string(pdb, NULL);

    // Environment variables
    pdb->access_origin = wrd_get_env_string("WARUDO_CORS", WRD_DEFAULT_CORS);
    pdb->log_level = wrd_get_env_int("WARUDO_LOG_LEVEL", WRD_DEFAULT_LOG_LEVEL);

    wrd_log_info(pdb, "Starting warudo %s\n", WRD_VERSION);
    wrd_log_info(pdb, "Access origin: %s\n", pdb->access_origin ? pdb->access_origin : "disabled");
    wrd_log_info(pdb, "Log level: %u\n", pdb->log_level);

    wrd_log_info(pdb, "Net buffer size: %u bytes\n", pdb->net_buffer.size);
    wrd_log_info(pdb, "Net input buffer size: %u bytes\n", pdb->net_buffer.size);
    wrd_log_info(pdb, "Net headers size: %u bytes\n", pdb->net_headers_buffer.size);
    wrd_log_info(pdb, "Loading DB: \"%s\"\n", filename);

    // Load database
    res = wrd_db_init(filename, pdb);

    if(res != WRD_OK) {
        wrd_close(pdb);

        return WRD_DB_INIT_ERROR;
    }

    *config = pdb;

    return WRD_OK;
}

wrd_code wrd_accept_connection(warudo *config) {
    CHECK_CONFIG

    wrd_code accepted = wrd_net_accept(config);

    if(accepted != WRD_OK) {
        wrd_log_error(config, "Request not accepted %llu\n", config->requests_count);

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
    fputs("--------->", stdout);
    // fputs(config->net_input_buffer.buffer, stdout);
    fputs("gatto", stdout);
    fputs("<---------", stdout);
    wrd_parse_query_string(config, NULL);
    wrd_log_info(config, "Accepted request %llu\n", config->requests_count);

    wrd_http_ok(config);
    wrd_http_printf(config, "Hello %s", "World!");
    wrd_http_puts(config, "Hello World");

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
