#ifndef WRD_QUERY_H
#define WRD_QUERY_H

#include <string.h>
#include <stdlib.h>
#include "warudo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, FN, LENGTH_1) if(!config->query.valid_##IDX && \
    strncmp(KEY, #IDX, LENGTH_1) == 0) { \
    config->query.##IDX = FN; \
    config->query.valid_##IDX = 1; }
#define WRD_GET_QUERY_ULLINT_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strtoll(VALUE, NULL, 10), LENGTH_1)
#define WRD_GET_QUERY_INT_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strtol(VALUE, NULL, 10), LENGTH_1)
#define WRD_GET_QUERY_STRING_VALUE(KEY, IDX, VALUE, LENGTH_1) WRD_GET_QUERY_VALUE(KEY, IDX, VALUE, strdup(VALUE), LENGTH_1)

#define WRD_FREE_QUERY_ULLINT_VALUE(NAME, DEFAULT_VALUE) config->query.##NAME = DEFAULT_VALUE; config->query.valid_##NAME = 0;
#define WRD_FREE_QUERY_INT_VALUE(NAME, DEFAULT_VALUE) config->query.##NAME = DEFAULT_VALUE; config->query.valid_##NAME = 0;
#define WRD_FREE_QUERY_STRING_VALUE(NAME) if(config->query.##NAME != NULL) free((void*)config->query.##NAME); \
    config->query.##NAME = NULL; \
    config->query.valid_##NAME = 0;
// #define WRD_FREE_QUERY_STRING_VALUES(NAME) for(int i = 0; i < config->query.valid_##NAME; ++i) { free((void*)config->query.##NAME[i]); \
//  config->query.##NAME[i] = NULL; } \
//  config->query.valid_##NAME = 0;

typedef int (*wrd_parse_formdata_callback)(const char*, long int, warudo*);

// Parse the query string
WRD_API wrd_code wrd_parse_query_string(warudo *config, char *query_string);
WRD_API char *wrd_url_decode(const char *input);

// Forms
WRD_API int wrd_is_valid_boundary(const char *boundary);
WRD_API const char *wrd_get_formdata_boundary(const char *content_type);
WRD_API wrd_code wrd_parse_formdata(const char *input, long int length, const char *boundary,
    wrd_parse_formdata_callback callback, warudo *config);

WRD_API long wrd_content_length(warudo *config);
WRD_API char *wrd_read_content(long int length, warudo *config);

#ifdef __cplusplus
}
#endif

#endif // WRD_QUERY_H
