#include "fcgi.h"

wrd_code wrd_fcgi_init(void) {
#ifdef WRD_USE_LIBFCGI
    int res = FCGX_Init();

    return res == 0 ? WRD_OK : WRD_FCGI_INIT_ERROR;
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

char *wrd_fcgi_get_param(const char *name, warudo *config) {
    if(!config) {
        return NULL;
    }

#ifdef WRD_USE_LIBFCGI
    return FCGX_GetParam(name, config->request.envp);
#endif

    return NULL;
}
