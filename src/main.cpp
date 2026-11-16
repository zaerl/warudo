/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include <stdlib.h>

#include "warudo.h"

int main(void) {
    warudo *config = NULL;
    wrd_code ret;

    config = static_cast<warudo *>(malloc(sizeof(warudo)));
    ret = wrd_server_init(config);

    if(ret != WRD_OK) {
        wrd_log_error(config, "Failed to initialize. Code %s\n", wrd_error_description(ret, 0));

        return ret;
    }

    wrd_log_info(config, "Bye %s!\n", WRD_VERSION);
    wrd_server_close(config);

    return WRD_OK;
}
