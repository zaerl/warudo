#ifndef WRD_SERVER_H
#define WRD_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "warudo.h"

// Init the server.
WRD_API wrd_code wrd_server_init(warudo *config);

// Save PID file.
WRD_API wrd_code wrd_server_save_pid(warudo *config);

// Server loop.
WRD_API wrd_code wrd_server_loop(warudo *config);

// Close the system
WRD_API wrd_code wrd_server_close(warudo *config);

// Accept an HTTP connection
// WRD_API wrd_code wrd_accept_connection(warudo *config);

// Run at the end of a connection
// WRD_API wrd_code wrd_after_connection(warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WRD_SERVER_H
