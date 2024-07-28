#include <stdio.h>

#include "code.h"
#include "db.h"
#include "http.h"
#include "log.h"
#include "output.h"
#include "query.h"
#include "server.h"
#include "sqlite3/sqlite3.h"

// /opt/homebrew/opt/nginx/bin/nginx -g "daemon off;"
// /opt/homebrew/opt/lighttpd/bin/lighttpd -D -f /opt/homebrew/etc/lighttpd/lighttpd.conf
// make && WRD_CORS="*" WRD_LOG_LEVEL=3 bin/warudo.cgi
int main(void) {
    warudo *config = NULL;
    wrd_code ret;

    config = malloc(sizeof(warudo));

    ret = wrd_server_init(config);

    if(ret != WRD_OK) {
        wrd_log_error(config, "Failed to Initialize. Code %s\n", wrd_error_description(ret, 0));

        return ret;
    }

    while(wrd_accept_connection(config) == WRD_OK) {
        wrd_http_parse_query_headers(config);
        wrd_log_info(config, u8"Accepted request %llu\n", config->requests_count);

        wrd_http_ok(config);
        wrd_http_puts(config, u8"{\"Hello\": \"World\"}");

        wrd_after_connection(config);
    }

    /*
    while(wrd_accept_connection(config) >= 0) {
        char **envp = config->request.envp;

        while(*envp) {
            wrd_log_debug(config, u8"%s\n", *envp);
            ++envp;
        }
        if(config->page == WRD_PAGE_APP) {
            ret = wrd_route_app(WRD_ENTRY_TYPE_DATA, config);
        } else if(config->page == WRD_PAGE_APP_KEYS) {
            ret = wrd_route_app_keys(config);
        } else if(config->page == WRD_PAGE_APP_VIEWS) {
            ret = wrd_route_app(WRD_ENTRY_TYPE_VIEW, config);
        } else if(config->page == WRD_PAGE_ROOT) {
            ret = wrd_route_home(config);
        } else {
            ret = wrd_http_not_found(config);
        }

        wrd_after_connection(config);

        if(ret == 0) {
            wrd_log_info(config, u8"Accepted request %llu\n", config->requests_count);
        } else {
            wrd_log_error(config, u8"Failed to accept request %llu. Code %s\n",
                config->requests_count, wrd_error_description(ret, 0));
        }
    }*/

    // Query string
    /*WRD_FREE_QUERY_ULLINT_VALUE(id, 0)
    WRD_FREE_QUERY_INT_VALUE(limit, WRD_DEFAULT_QUERY_LIMIT)
    WRD_FREE_QUERY_ULLINT_VALUE(offset, 0)
    WRD_FREE_QUERY_ULLINT_VALUE(multi, WRD_DEFAULT_QUERY_MULTI)
    WRD_FREE_QUERY_STRING_VALUE(key)
    WRD_FREE_QUERY_STRING_VALUE(value)
    WRD_FREE_QUERY_STRING_VALUE(orderby)
    WRD_FREE_QUERY_STRING_VALUE(sort)*/
    // WRD_FREE_QUERY_STRING_VALUES(keys)
    // WRD_FREE_QUERY_STRING_VALUES(values)


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

    wrd_log_info(config, u8"Bye %s!\n", WRD_VERSION);
    wrd_server_close(config);

    return WRD_OK;
}
