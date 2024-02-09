#ifndef WRD_QUERY_H
#define WRD_QUERY_H

#include <string.h>
#include <stdlib.h>
#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, FN, LENGTH_1) if(!config->query_valid_##IDX && \
    strncmp(KEY, #IDX, LENGTH_1) == 0) { \
    config->query_##IDX = FN; \
    config->query_valid_##IDX = 1; }
#define WRD_GET_QUERY_ULLINT_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strtoll(VALUE, NULL, 10), LENGTH_1)
#define WRD_GET_QUERY_INT_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strtol(VALUE, NULL, 10), LENGTH_1)
#define WRD_GET_QUERY_STRING_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strdup(VALUE), LENGTH_1)

#define WRD_FREE_QUERY_ULLINT_VALUE(NAME, DEFAULT_VALUE) config->query_##NAME = DEFAULT_VALUE; config->query_valid_##NAME = 0;
#define WRD_FREE_QUERY_INT_VALUE(NAME, DEFAULT_VALUE) config->query_##NAME = DEFAULT_VALUE; config->query_valid_##NAME = 0;
#define WRD_FREE_QUERY_STRING_VALUE(NAME) if(config->query_##NAME != NULL) free((void*)config->query_##NAME); \
    config->query_##NAME = NULL; \
    config->query_valid_##NAME = 0;
// #define WRD_FREE_QUERY_STRING_VALUES(NAME) for(int i = 0; i < config->query_valid_##NAME; ++i) { free((void*)config->query_##NAME[i]); \
//  config->query_##NAME[i] = NULL; } \
//  config->query_valid_##NAME = 0;

// Parse the query string
wrd_code wrd_parse_query_string(warudo *config, char *query_string);

#ifdef __cplusplus
}
#endif

#endif // WRD_QUERY_H
