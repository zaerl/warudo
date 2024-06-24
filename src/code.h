#ifndef WRD_CODE_H
#define WRD_CODE_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO: check constr char
WRD_API const char *wrd_error_description(wrd_code code, int extended);

#ifdef __cplusplus
}
#endif

#endif // WRD_CODE_H
