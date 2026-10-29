/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <stdlib.h>
#include <string.h>

#include "app.h"

const wrd_app wrd_entries_app = {
    .init = wrd_app_init,
    .close = wrd_app_close,
};

WRD_API wrd_code wrd_app_init(warudo *config) {
    CHECK_CONFIG

    config->app_data = calloc(1, sizeof(wrd_app_data));

    if(config->app_data == NULL) {
        return WRD_MEMORY_ERROR;
    }

    wrd_code ret = wrd_db_init(config->db_path, config);

    if(ret != WRD_OK) {
        return WRD_DB_INIT_ERROR;
    }

    wrd_route_add(config, "/", wrd_route_home);
    wrd_route_add(config, "/app/entries", wrd_route_entries);
    wrd_route_add(config, "/app/keys", wrd_route_app_keys);
    wrd_route_add(config, "/app/dashboards", wrd_route_dashboards);

    return WRD_OK;
}

WRD_API wrd_code wrd_app_close(warudo *config) {
    CHECK_APP

    wrd_code ret = wrd_db_close(config);

    free(config->app_data);
    config->app_data = NULL;

    return ret;
}

WRD_API wrd_code wrd_app_parse_query_string(wrd_query *query, char *query_string) {
    if(query == NULL) {
        return WRD_ERROR;
    }

    // Reset to defaults.
    query->id = 0;
    query->offset = 0;
    query->limit = WRD_DEFAULT_QUERY_LIMIT;
    query->multi = WRD_DEFAULT_QUERY_MULTI;
    query->key = NULL;
    query->value = NULL;
    query->orderby = NULL;
    query->sort = NULL;

    if(query_string == NULL || query_string[0] == '\0') {
        return WRD_OK;
    }

    char *saveptr;
    char *parameter = strtok_r(query_string, "&", &saveptr);

    while(parameter != NULL) {
        char *delimiter = strchr(parameter, '=');

        if(delimiter != NULL) {
            int name_len = delimiter - parameter;
            char *value = delimiter + 1;

            if(name_len == 5 && strncmp(parameter, "limit", 5) == 0) {
                query->limit = (unsigned int)strtol(value, NULL, 10);
            } else if(name_len == 6 && strncmp(parameter, "offset", 6) == 0) {
                query->offset = strtoull(value, NULL, 10);
            } else if(name_len == 2 && strncmp(parameter, "id", 2) == 0) {
                query->id = strtoull(value, NULL, 10);
            } else if(name_len == 5 && strncmp(parameter, "multi", 5) == 0) {
                query->multi = (unsigned int)strtol(value, NULL, 10);
            } else if(name_len == 3 && strncmp(parameter, "key", 3) == 0) {
                query->key = strdup(value);
            } else if(name_len == 5 && strncmp(parameter, "value", 5) == 0) {
                query->value = strdup(value);
            } else if(name_len == 7 && strncmp(parameter, "orderby", 7) == 0) {
                query->orderby = strdup(value);
            } else if(name_len == 4 && strncmp(parameter, "sort", 4) == 0) {
                query->sort = strdup(value);
            }
        }

        parameter = strtok_r(NULL, "&", &saveptr);
    }

    return WRD_OK;
}

WRD_API wrd_code wrd_app_free_query(wrd_query *query) {
    if(query == NULL) {
        return WRD_ERROR;
    }

    free((void*)query->key);
    free((void*)query->value);
    free((void*)query->orderby);
    free((void*)query->sort);

    query->key = NULL;
    query->value = NULL;
    query->orderby = NULL;
    query->sort = NULL;

    return WRD_OK;
}

WRD_API wrd_code wrd_route_home(warudo *config) {
    CHECK_CONFIG

    return wrd_http_server_error(config, "Silence is golden");
}

static wrd_code wrd_route_app(int entry_type, warudo *config) {
    CHECK_APP

    wrd_code ret;
    wrd_query *query = &WRD_APP(config)->query;

    wrd_app_parse_query_string(query, config->request_query_string);

    if(config->request_method == WRD_REQUEST_GET) {
        ret = wrd_get_entries(entry_type, config);
    } else if(config->request_method == WRD_REQUEST_POST) {
        if(query->multi && entry_type == WRD_ENTRY_TYPE_DATA) {
            ret = wrd_add_entries(entry_type, config);
        } else {
            ret = wrd_add_entry(entry_type, config);
        }
    } else {
        ret = wrd_http_not_allowed(config, "GET, POST");
    }

    wrd_app_free_query(query);

    return ret;
}

WRD_API wrd_code wrd_route_entries(warudo *config) {
    return wrd_route_app(WRD_ENTRY_TYPE_DATA, config);
}

WRD_API wrd_code wrd_route_dashboards(warudo *config) {
    return wrd_route_app(WRD_ENTRY_TYPE_VIEW, config);
}

WRD_API wrd_code wrd_route_app_keys(warudo *config) {
    CHECK_APP

    if(config->request_method == WRD_REQUEST_GET) {
        return wrd_get_keys(config);
    }

    return wrd_http_not_allowed(config, "GET");
}

WRD_API wrd_code wrd_http_created(warudo *config, unsigned long long int id) {
    CHECK_CONFIG

    if(config->client_fd <= 0) {
        return WRD_ERROR;
    }

    wrd_http_header(config, "201 Created", "application/json");
    wrd_http_printf(config, u8"{\"status\":\"success\",\"id\":%lld}", id);

    return WRD_OK;
}

WRD_API wrd_code wrd_http_multi_created(warudo *config, unsigned long int count) {
    CHECK_CONFIG

    if(config->client_fd <= 0) {
        return WRD_ERROR;
    }

    wrd_http_header(config, "201 Created", "application/json");
    wrd_http_printf(config, u8"{\"status\":\"success\",\"count\":%lld}", count);

    return WRD_OK;
}
