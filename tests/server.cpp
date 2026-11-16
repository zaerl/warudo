/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include "test.h"

ATT_TEST(server) {
    ASSERT_CODE(wrd_server_init(NULL, NULL), WRD_ERROR, "wrd_server_init NULL")
    // ASSERT_CODE(wrd_accept_connection(NULL), WRD_ERROR, "wrd_accept_connection NULL")
    // ASSERT_CODE(wrd_accept_connection(NULL), WRD_ERROR, "wrd_accept_connection NULL")
    // ASSERT_CODE(wrd_after_connection(NULL), WRD_ERROR, "wrd_after_connection NULL")
    ASSERT_CODE(wrd_server_close(NULL), WRD_ERROR, "wrd_server_close NULL")
}
