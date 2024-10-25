#ifndef WRD_FS_H
#define WRD_FS_H

#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

// Read a file into a buffer.
WRD_API wrd_code wrd_read_file(const char *file_path, void **file_buffer,
    long *file_size, long max_size);

#ifdef __cplusplus
}
#endif

#endif // WRD_FS_H
