#include <stdarg.h>
#include <stdio.h>

#include "fcgi.h"

wrd_code wrd_fcgi_init(void) {
#ifdef WRD_USE_LIBFCGI
    int res = FCGX_Init();

    return res == 0 ? WRD_OK : WRD_FCGI_INIT_ERROR;
#endif

    return WRD_OK;
}

wrd_code wrd_fcgi_open_socket(const char *path, int backlog) {
    int socket = -1;

#ifdef WRD_USE_LIBFCGI
    socket = FCGX_OpenSocket(path, backlog);
#endif

    return socket == -1 ? WRD_SOCKET_ERROR : socket;
}

wrd_code wrd_fcgi_init_request(warudo *config) {
    CHECK_CONFIG

    int res = WRD_OK;

#ifdef WRD_USE_LIBFCGI
    res = FCGX_InitRequest(&config->request, config->socket, 0);

    return res == 0 ? WRD_OK : WRD_INIT_REQUEST_ERROR;
#endif

    return res;
}

wrd_code wrd_fcgi_accept(warudo *config) {
    CHECK_CONFIG

    int accepted = WRD_OK;

#ifdef WRD_USE_LIBFCGI
    if(FCGX_Accept_r(&config->request) < 0) {
        accepted = WRD_ACCEPT_ERROR;
    }
#endif

    return accepted;
}

wrd_code wrd_fcgi_finish_request(warudo *config) {
    CHECK_CONFIG

    int res = WRD_OK;

#ifdef WRD_USE_LIBFCGI
    FCGX_Finish_r(&config->request);
#endif

    return res;
}

wrd_code wrd_fcgi_free_request(warudo *config) {
    CHECK_CONFIG

    int res = WRD_OK;

#ifdef WRD_USE_LIBFCGI
    FCGX_Free(&config->request, 1);
#endif

    return res;
}

char *wrd_fcgi_get_param(const char *name, warudo *config) {
    if(!config) {
        return NULL;
    }

#ifdef WRD_USE_LIBFCGI
    return FCGX_GetParam(name, config->request.envp);
#endif

    return NULL;
}

wrd_code wrd_fcgi_getstr(char *str, int length, warudo *config) {
    CHECK_CONFIG

#ifdef WRD_USE_LIBFCGI
    FCGX_GetStr(str, length, config->request.in);
#endif

    return WRD_OK;
}

wrd_code wrd_fcgi_puts(const char *str, warudo *config) {
    CHECK_CONFIG

#ifdef WRD_USE_LIBFCGI
    FCGX_PutS(str, config->request.out);
#endif

    return WRD_OK;
}

wrd_code wrd_fcgi_printf(warudo *config, const char *format, ...) {
    CHECK_CONFIG

#ifdef WRD_USE_LIBFCGI
    va_list args;
    va_start(args, format);
    int res = FCGX_VFPrintF(config->request.out, format, args);
    va_end(args);

    return res == EOF ? WRD_ERROR : WRD_OK;
#endif

    return WRD_OK;
}
