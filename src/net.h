#ifndef WRD_NET_H
#define WRD_NET_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

WRD_API wrd_code wrd_net_init(warudo *config, int backlog);
WRD_API wrd_code wrd_net_close(warudo *config);
WRD_API wrd_code wrd_net_accept(warudo *config);
WRD_API wrd_code wrd_net_finish_request(warudo *config);
WRD_API wrd_code wrd_net_printf(warudo *config, wrd_buffer *buffer, const char *format, ...);
WRD_API wrd_code wrd_net_read(warudo *config);
WRD_API wrd_code wrd_net_send(warudo *config, wrd_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif // WRD_NET_H
