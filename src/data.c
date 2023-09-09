#include "data.h"

size_t wrd_debug_dump(const char* text, FILE* stream, unsigned char* ptr, size_t size) {
    size_t i;
    size_t c;
    unsigned int width = 0x10;
    size_t ret = 0;

    if(ptr == NULL || stream == NULL) {
        return 0;
    }

    if(text != NULL) {
        fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n", text, (long)size, (long)size);
    }

    for(i = 0; i < size; i += width) {
        fprintf(stream, "%4.4lx: ", (long)i);

        // Show hex to the left
        for(c = 0; c < width; ++c) {
            if(i + c < size) {
                fprintf(stream, "%02x ", ptr[i + c]);
            } else {
                fputs("   ", stream);
            }
        }

        // Show data on the right
        for(c = 0; (c < width) && (i + c < size); ++c) {
            char x = '.';

            if(ptr[i + c] >= 0x20 && ptr[i + c] < 0x80) {
                x = ptr[i + c];
                ++ret;
            }

            fputc(x, stream);
        }

        fputc('\n', stream);
    }

    return ret;
}
