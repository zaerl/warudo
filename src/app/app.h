/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#ifndef WARUDO_APP_H
#define WARUDO_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../warudo.h"
#include "../sqlite3/sqlite3.h"

// Example JSON entries application, built on top of the Warudo server.

#define WRD_AUTH_DEFAULT_USER "admin"
#define WRD_AUTH_DEFAULT_PASS "admin"
#define WRD_AUTH_DEFAULT_ROLE "admin"

// Database
#define WRD_DASHBOARDS_TABLE "dashboards"
#define WRD_ENTRIES_TABLE "entries"
#define WRD_AUTH_TABLE "http_auth"

#define WRD_ENTRY_TYPE_DATA 0
#define WRD_ENTRY_TYPE_VIEW 1

// Query
#define WRD_DEFAULT_QUERY_LIMIT 10
#define WRD_DEFAULT_QUERY_MULTI 0

typedef struct wrd_query {
    unsigned long long int id;
    unsigned long long int offset;
    unsigned int limit;
    unsigned int multi;
    const char *key;
    const char *value;
    const char *orderby;
    const char *sort;
} wrd_query;

// Per-process application state, stored in warudo.app_data.
typedef struct wrd_app_data {
    sqlite3 *db;
    sqlite3_stmt *insert_stmt;
    sqlite3_stmt *insert_dashboard_stmt;
    sqlite3_stmt *add_index_stmt;
    sqlite3_stmt *parse_json_stmt;
    wrd_query query;
} wrd_app_data;

#define WRD_APP(config) ((wrd_app_data*)(config)->app_data)
#define CHECK_APP if(!config || config->status != WRD_LOADED || !config->app_data) return WRD_ERROR;

extern const wrd_app wrd_entries_app;

// App
WRD_API wrd_code wrd_app_init(warudo *config);
WRD_API wrd_code wrd_app_close(warudo *config);
WRD_API wrd_code wrd_app_parse_query_string(wrd_query *query, char *query_string);
WRD_API wrd_code wrd_app_free_query(wrd_query *query);
WRD_API wrd_code wrd_route_home(warudo *config);
WRD_API wrd_code wrd_route_entries(warudo *config);
WRD_API wrd_code wrd_route_dashboards(warudo *config);
WRD_API wrd_code wrd_route_app_keys(warudo *config);
WRD_API wrd_code wrd_http_created(warudo *config, unsigned long long int id);
WRD_API wrd_code wrd_http_multi_created(warudo *config, unsigned long int count);

// Database
WRD_API wrd_code wrd_db_init(const char *filename, warudo *config);
WRD_API wrd_code wrd_db_close(warudo *config);
unsigned long long wrd_last_insert_rowid(warudo *config);
WRD_API const char *wrd_db_order_by(const char *value);
WRD_API const char *wrd_db_sort(const char *value);
WRD_API wrd_code wrd_parse_json(warudo *config);
WRD_API wrd_code wrd_add_index(const char *filename, warudo *config);
WRD_API wrd_code wrd_add_entry(int entry_type, warudo *config);
WRD_API wrd_code wrd_formdata_callback(const char *input, long int length, warudo *config);
WRD_API wrd_code wrd_add_entries(int entry_type, warudo *config);
WRD_API wrd_code wrd_get_entries(int entry_type, warudo *config);
WRD_API wrd_code wrd_get_keys(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WARUDO_APP_H
