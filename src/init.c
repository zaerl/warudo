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
    char *env;
    int res;

    pdb = malloc(sizeof(warudo));
    pdb->columns_count = 0;
    pdb->requests_count = 0;

    pdb->net_buffer.buffer = NULL;
    pdb->net_headers_buffer.buffer = NULL;

#ifdef WRD_TIMING
    pdb->timing_count = 0;
    pdb->timing_end_count = 0;
#endif

    int backlog = WRD_LISTEN_BACKLOG;
    env = getenv("WARUDO_LISTEN_BACKLOG");

    if(env != NULL) {
        backlog = atoi(env);
    }

    // Load FCGI
    res = wrd_net_init(pdb, backlog);

    if(res != WRD_OK) {
        wrd_close(pdb);

        return res;
    }

    pdb->page = -1;
    pdb->request_method = WRD_REQUEST_UNKNOWN;
    pdb->script_name = NULL;
    pdb->query_string = NULL;

    // Query string
    pdb->query_id = 0;
    pdb->query_offset = 0;
    pdb->query_multi = 0;
    pdb->query_limit = 0;
    pdb->query_key = NULL;
    pdb->query_value = NULL;
    pdb->query_orderby = NULL;
    pdb->query_sort = NULL;

    // Environment variables
    pdb->access_origin = wrd_get_env_string("WARUDO_CORS", WRD_DEFAULT_CORS);
    pdb->log_level = wrd_get_env_int("WARUDO_LOG_LEVEL", WRD_DEFAULT_LOG_LEVEL);

    wrd_log_info(pdb, "Starting warudo %s\n", WRD_VERSION);
    wrd_log_info(pdb, "Access origin: %s\n", pdb->access_origin ? pdb->access_origin : "disabled");
    wrd_log_info(pdb, "Log level: %u\n", pdb->log_level);

    // HTTP headers memory.
    pdb->net_headers_buffer.buffer = calloc(1, pdb->net_headers_buffer.size);
    pdb->net_headers_buffer.size = WRD_NET_HEADERS_BUFFER_SIZE;
    pdb->net_buffer.size = wrd_get_env_int("WRD_NET_BUFFER_SIZE", WRD_NET_BUFFER_SIZE);

    // HTTP content memory.
    pdb->net_buffer.size *= 1048576;
    pdb->net_buffer.buffer = calloc(1, pdb->net_buffer.size);

    wrd_log_info(pdb, "Net buffer size: %u bytes\n", pdb->net_buffer.size);
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

    // Query string
    WRD_FREE_QUERY_ULLINT_VALUE(id, 0)
    WRD_FREE_QUERY_INT_VALUE(limit, WRD_DEFAULT_QUERY_LIMIT)
    WRD_FREE_QUERY_ULLINT_VALUE(offset, 0)
    WRD_FREE_QUERY_ULLINT_VALUE(multi, WRD_DEFAULT_QUERY_MULTI)
    WRD_FREE_QUERY_STRING_VALUE(key)
    WRD_FREE_QUERY_STRING_VALUE(value)
    WRD_FREE_QUERY_STRING_VALUE(orderby)
    WRD_FREE_QUERY_STRING_VALUE(sort)
    // WRD_FREE_QUERY_STRING_VALUES(keys)
    // WRD_FREE_QUERY_STRING_VALUES(values)

    wrd_log_info(config, "Accepted request %llu\n", config->requests_count);

    wrd_http_ok(config);
    wrd_http_printf(config, "Hello %s", "World!");
    wrd_http_puts(config, "Hello World");

    // Read the HTTP request
    /*read(config->client_fd, buffer, 1024);
    puts(buffer);

    // Send a response
    send(config->client_fd, response, strlen(response), 0);
    puts("<<-Response sent\n");*/

    /*
    const char *script_name = wrd_fcgi_get_param("SCRIPT_NAME", config);
    const char *query_string = wrd_fcgi_get_param("QUERY_STRING", config);
    const char *request_method = wrd_fcgi_get_param("REQUEST_METHOD", config);

    if(script_name != NULL && strcmp(script_name, "/") == 0) {
        config->page = WRD_PAGE_ROOT;
    } else if(script_name != NULL && strcmp(script_name, "/app/entries") == 0) {
        config->page = WRD_PAGE_APP;
    } else if(script_name != NULL && strcmp(script_name, "/app/keys") == 0) {
        config->page = WRD_PAGE_APP_KEYS;
    } else if(script_name != NULL && strcmp(script_name, "/app/dashboards") == 0) {
        config->page = WRD_PAGE_APP_VIEWS;
    } else {
        config->page = WRD_PAGE_NOT_FOUND;
    }

    if(request_method != NULL) {
        if(strcmp(request_method, "GET") == 0) {
            config->request_method = WRD_REQUEST_GET;
        } else if(strcmp(request_method, "POST") == 0) {
            config->request_method = WRD_REQUEST_POST;
        } else {
            config->request_method = WRD_REQUEST_UNKNOWN;
        }
    }

    if(query_string != NULL) {
        wrd_parse_query_string(config, (char*)query_string);
    }

    config->script_name = script_name;
    config->query_string = query_string;*/

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

    if(config->net_buffer.buffer != NULL) {
        free(config->net_buffer.buffer);
    }

    if(config->net_headers_buffer.buffer != NULL) {
        free(config->net_headers_buffer.buffer);
    }

    free(config);

    return WRD_OK;
}
