#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "auth.h"

char *wrd_base64_encode(const char* input) {
    if(input == NULL) {
        return NULL;
    }

    const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t input_length = strlen(input);
    size_t ret_length = 4 * ((input_length + 2) / 3);
    char *res = (char*)malloc(ret_length + 1);

    if(res == NULL) {
        return NULL;
    }

    const uint8_t *data = (const uint8_t*)input;
    size_t result_index = 0;
    int pad_count = input_length % 3;

    for(size_t x = 0; x < input_length; x += 3) {
        // Three 8-bit (ASCII) characters become one 24-bit number
        uint32_t n = ((uint32_t)data[x]) << 16;

        if((x + 1) < input_length) {
            n += ((uint32_t)data[x + 1]) << 8;
        }

        if((x + 2) < input_length) {
           n += data[x + 2];
        }

        uint8_t n0 = (uint8_t)(n >> 18) & 63;
        uint8_t n1 = (uint8_t)(n >> 12) & 63;
        uint8_t n2 = (uint8_t)(n >> 6) & 63;
        uint8_t n3 = (uint8_t)n & 63;

        res[result_index++] = base64chars[n0];
        res[result_index++] = base64chars[n1];

       if((x + 1) < input_length) {
           res[result_index++] = base64chars[n2];
       }

       if((x + 2) < input_length) {
           res[result_index++] = base64chars[n3];
       }
    }

    if(pad_count > 0) {
       for(; pad_count < 3; pad_count++) {
           res[result_index++] = '=';
       }
    }

    res[result_index] = '\0';

    return res;
}

uint8_t wrd_base64_decode_char(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    } else if (c >= '0' && c <= '9') {
        return c - '0' + 52;
    } else if (c == '+') {
        return 62;
    } else if (c == '/') {
        return 63;
    } else if (c == '=') {
        return 0; // Padding character
    } else {
        return 255; // Invalid character
    }
}

WRD_API char* wrd_base64_decode(const char *input) {
    if(input == NULL) {
        return NULL;
    }

    size_t input_length = strlen(input);

    // Invalid length
    if(input_length % 4 != 0) {
        return NULL;
    }

    // Base64 encodes 3 bytes into 4 characters
    size_t ret_length = 3 * (input_length / 4);
    char *ret = malloc(ret_length + 1);

    if(ret == NULL) {
        return NULL;
    }

    size_t i = 0;
    size_t j = 0;

    while(i < input_length) {
        uint32_t n1 = wrd_base64_decode_char(input[i++]);
        uint32_t n2 = wrd_base64_decode_char(input[i++]);
        uint32_t n3 = wrd_base64_decode_char(input[i++]);
        uint32_t n4 = wrd_base64_decode_char(input[i++]);

        if(n1 == 255 || n2 == 255 || n3 == 255 || n4 == 255) {
            free(ret);

            return NULL;
        }

        uint32_t triple = (n1 << 18) + (n2 << 12) + (n3 << 6) + n4;

        if(j < ret_length) {
            ret[j++] = (triple >> 16) & 0xFF;
        }

        if(j < ret_length) {
            ret[j++] = (triple >> 8) & 0xFF;
        }

        if(j < ret_length) {
            ret[j++] = triple & 0xFF;
        }
    }

    ret[j] = '\0';

    return ret;
}
