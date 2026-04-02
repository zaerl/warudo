/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include "warudo.h"

WRD_API wrd_code wrd_route_home(warudo *config) {
    CHECK_CONFIG

    return wrd_http_server_error(config, "Silence is golden");
}
