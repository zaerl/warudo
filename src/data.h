#ifndef WRD_CODE_H
#define WRD_CODE_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t wrd_debug_dump(const char* text, FILE* stream, unsigned char* ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif // WRD_CODE_H
