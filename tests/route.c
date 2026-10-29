#include "test.h"

static wrd_code mock_handler_a(warudo *config) {
    return WRD_OK;
}

static wrd_code mock_handler_b(warudo *config) {
    return WRD_DEFAULT;
}

void *test_route(void *arg) {
    ASSERT_CODE(wrd_route_add(NULL, "/", mock_handler_a), WRD_ERROR, "wrd_route_add NULL")
    ATT_ASSERT(wrd_route_find(NULL, "/") == NULL, 1, "wrd_route_find NULL");
    ASSERT_CODE(wrd_route_dispatch(NULL), WRD_ERROR, "wrd_route_dispatch NULL")

    MOCK_CONFIG

    ATT_ASSERT(config.routes_count, 0, "No routes by default");
    ASSERT_CODE(wrd_route_add(&config, NULL, mock_handler_a), WRD_ERROR, "NULL path")
    ASSERT_CODE(wrd_route_add(&config, "/", NULL), WRD_ERROR, "NULL handler")
    ATT_ASSERT(wrd_route_find(&config, NULL) == NULL, 1, "Find NULL path");

    // Add and find.
    ASSERT_CODE(wrd_route_add(&config, "/a", mock_handler_a), WRD_OK, "Add /a")
    ASSERT_CODE(wrd_route_add(&config, "/b", mock_handler_b), WRD_OK, "Add /b")
    ATT_ASSERT(config.routes_count, 2, "Two routes");
    ATT_ASSERT(wrd_route_find(&config, "/a") == mock_handler_a, 1, "Find /a");
    ATT_ASSERT(wrd_route_find(&config, "/b") == mock_handler_b, 1, "Find /b");
    ATT_ASSERT(wrd_route_find(&config, "/c") == NULL, 1, "Unknown path");
    ATT_ASSERT(wrd_route_find(&config, "/a/") == NULL, 1, "Paths match exactly");

    // Re-adding a path replaces its handler.
    ASSERT_CODE(wrd_route_add(&config, "/a", mock_handler_b), WRD_OK, "Replace /a")
    ATT_ASSERT(config.routes_count, 2, "Still two routes");
    ATT_ASSERT(wrd_route_find(&config, "/a") == mock_handler_b, 1, "Find replaced /a");

    // Dispatch calls the handler registered for the request path.
    strcpy(config.request_path, "/b");
    ASSERT_CODE(wrd_route_dispatch(&config), WRD_DEFAULT, "Dispatch /b")

    // The table is bounded.
    static char paths[WRD_MAX_ROUTES][8];

    for(unsigned int i = config.routes_count; i < WRD_MAX_ROUTES; ++i) {
        snprintf(paths[i], sizeof(paths[i]), "/%u", i);
        wrd_route_add(&config, paths[i], mock_handler_a);
    }

    ATT_ASSERT(config.routes_count, WRD_MAX_ROUTES, "Routes table full");
    ASSERT_CODE(wrd_route_add(&config, "/full", mock_handler_a), WRD_ERROR, "Add to full table")

    return NULL;
}
