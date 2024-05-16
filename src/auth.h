#ifndef WRD_AUTH_H
#define WRD_AUTH_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

//char* wrd_base64_encode(const char *input);
char* wrd_base64_encode(const char *input);
char* wrd_base64_decode(const char *input);

#ifdef __cplusplus
}
#endif

#endif // WRD_AUTH_H
