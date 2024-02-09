#ifndef WRD_NET_H
#define WRD_NET_H

#include <sys/socket.h>

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

wrd_code wrd_net_init(warudo *config, int backlog);
wrd_code wrd_net_close(warudo *config);
wrd_code wrd_net_accept(warudo *config);
wrd_code wrd_net_finish_request(warudo *config);
wrd_code wrd_net_printf(warudo *config, wrd_buffer *buffer, const char *format, ...);
wrd_code wrd_net_read(warudo *config);
wrd_code wrd_net_send(warudo *config, wrd_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif // WRD_NET_H
