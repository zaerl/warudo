#include <stdio.h>
#include <stdlib.h>

#include "fs.h"

// Read a file into a buffer.
WRD_API wrd_code wrd_read_file(const char *file_path, void **file_buffer,
    long *file_size, long max_size) {
    if(file_path == NULL) {
        // Nothing to load, return.
        return WRD_ERROR;
    }

    if(max_size == 0) {
        *file_buffer = NULL;

        return WRD_OK;
    }

    FILE *file = fopen(file_path, "r");

    if(!file) {
        return WRD_FILE_ERROR;
    }

    fseek(file, 0, SEEK_END);
    long fs_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if(fs_size == 0) {
        *file_size = 0;
        *file_buffer = NULL;

        return WRD_OK;
    }

    *file_size = fs_size;
    void *fs_buffer = malloc(fs_size);

    if(!fs_buffer) {
        fclose(file);

        return WRD_MEMORY_ERROR;
    }

    *file_size = fs_size;
    *file_buffer = fs_buffer;

    fread(*file_buffer, 1, *file_size, file);
    fclose(file);

    return WRD_OK;
}
