#include <stdio.h>
#include <stdlib.h>
#include "config.h"

void warudo_read_config(void) {
    const char* path = getenv("WARUDO_CONFIG_FILE");

    if (path == NULL) {
        path = WARUDO_CONFIG_FILE;
    }

    /*FILE* file = fopen(path, "r");

    // Read and process the configuration data
    char key[100];
    char value[100];

    while (fscanf(file, "%s = %s", key, value) == 2) {
        printf("Key: %s, Value: %s\n", key, value);
    }

    fclose(file);*/
}