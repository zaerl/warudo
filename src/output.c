#include <stdlib.h>
#include <string.h>

#include "output.h"

// Function to escape special characters in a string for HTML
WRD_API char *wrd_escape_html(const char *input) {
    if(input == NULL) {
        return NULL;
    }

    size_t input_length = strlen(input);
    char *escaped = malloc(input_length * 6 + 1);
    size_t j = 0;

    if(escaped == NULL) {
        return NULL;
    }

    for(size_t i = 0; i < input_length; ++i) {
        switch(input[i]) {
            case '&':
                strncpy(&escaped[j], "&amp;", 5);
                j += 5;
                break;
            case '<':
                strncpy(&escaped[j], "&lt;", 4);
                j += 4;
                break;
            case '>':
                strncpy(&escaped[j], "&gt;", 4);
                j += 4;
                break;
            case '"':
                strncpy(&escaped[j], "&quot;", 6);
                j += 6;
                break;
            case '\'':
                strncpy(&escaped[j], "&apos;", 6);
                j += 6;
                break;
            default:
                escaped[j] = input[i];
                ++j;
        }
    }

    escaped[j] = '\0';

    return escaped;
}
