#ifndef WRD_HPACK_H
#define WRD_HPACK_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

wrd_code wrd_compress_header(const char *name, const char *value, char *output);
wrd_code wrd_decompress_header(const char *input, char *name, char *value);

#ifdef __cplusplus
}
#endif

#endif // WRD_HPACK_H
