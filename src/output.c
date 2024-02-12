#include <stdlib.h>
#include <string.h>

#include "http.h"
#include "output.h"

// Function to escape special characters in a string for HTML
char *wrd_escape_html(const char *input) {
    if(input == NULL) {
        return NULL;
    }

    size_t len = strlen(input);
    size_t escaped_len = len * 6 + 1;
    char *escaped = malloc(len);

    if(escaped == NULL) {
        return NULL;
    }

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        switch (input[i]) {
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
                strncpy(&escaped[j], "&#39;", 5);
                j += 5;
                break;
            default:
                escaped[j] = input[i];
                j++;
                break;
        }

        if(j >= escaped_len - 6) {
            // Increase buffer size if necessary
            escaped_len *= 2;
            char *temp = realloc(escaped, escaped_len);

            if(temp == NULL) {
                free(escaped);

                return NULL;
            }

            escaped = temp;
        }
    }

    escaped[j] = '\0';

    return escaped;
}
