#include "query.h"

wrd_code wrd_parse_query_string(warudo *config, char *query_string) {
    CHECK_CONFIG

    if(query_string == NULL) {
        return WRD_EMPTY_QUERY_STRING_ERROR;
    }

    char *saveptr;
    char *parameter = strtok_r(query_string, "&", &saveptr);

    while(parameter != NULL) {
        char *delimiter = strchr(parameter, '=');

        if(delimiter != NULL) {
            int delimiter_idx = delimiter - parameter;
            int length_1 = delimiter_idx;
            // int length_2 = strlen(parameter) - delimiter_idx - 1;
            char *value = delimiter + 1;

            if(value != NULL) {
                WRD_GET_QUERY_ULLINT_VALUE(parameter, limit, value, length_1)
                else WRD_GET_QUERY_INT_VALUE(parameter, offset, value, length_1)
                else WRD_GET_QUERY_ULLINT_VALUE(parameter, id, value, length_1)
                else WRD_GET_QUERY_INT_VALUE(parameter, multi, value, length_1)
                else WRD_GET_QUERY_STRING_VALUE(parameter, key, value, length_1)
                else WRD_GET_QUERY_STRING_VALUE(parameter, value, value, length_1)
                else WRD_GET_QUERY_STRING_VALUE(parameter, orderby, value, length_1)
                else WRD_GET_QUERY_STRING_VALUE(parameter, sort, value, length_1)
            }
        }

        parameter = strtok_r(NULL, "&", &saveptr);
    }

    return WRD_OK;
}
