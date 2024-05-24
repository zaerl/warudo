#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conf.h"

#define YYCTYPE char
#define YYCURSOR config_cursor
#define YYLIMIT config_limit
#define YYMARKER config_marker
#define YYFILL(n) { if (config_fill(n)) return; }

wrd_code wrd_load_conf(const char *config) {
    char *config_cursor = (char*)config;
    char *config_limit = (char *)config + strlen(config);
    char *config_marker = NULL;

    while (config_cursor < config_limit) {
        char *name_start, *name_end;
        char *value_start, *value_end;
        /*!re2c
            re2c:define:YYCTYPE = char;
            re2c:yyfill:enable = 0;

            [ \t]*       { /* Ignore leading whitespace */ }
            '#' [^\n]*   { continue; }
            [^= \t\r\n]+ { name_start = YYCURSOR; YYMARKER; name_end = YYCURSOR; }
            [ \t]*       { /* Ignore spaces before = */ }
            '='          { /* Matched equal sign */ }
            [ \t]*       { /* Ignore spaces after = */ }
            [^\r\n]+     { value_start = YYCURSOR; YYMARKER; value_end = YYCURSOR; }
            '\n'         { /* End of line */ }
            [\r\n]       { /* Ignore standalone newlines */ }
            [^\x00]      { /* End of input */ return; }
        */
        *name_end = '\0';
        *value_end = '\0';

        // Print the name and value.
        printf("Name: '%s', Value: '%s'\n", name_start, value_start);

        config_cursor = value_end + 1;
    }

    return WRD_OK;
}
