<?php
// phpcs:ignoreFile

if(php_sapi_name() !== 'cli') {
    echo 'This script can only be run from the command line.';
    exit(1);
}

if($argc < 2 || !in_array($argv[1], ['h', 'c', 'conf'])) {
    echo "Usage: php generate-config.php h|c|conf\n";
    exit(1);
}

$map = [
    'Database',
    ['db_path', 'file:warudo.db'],
    'Log level can be one of the following [0, 1, 2, 3]: no_log, info, error, debug',
    ['log_level', 0, ['no_log', 'info', 'error', 'debug']],
    'Net',
    ['access_origin'],
    ['listen_backlog', 1024],
    ['max_columns', 64],
    ['net_buffer_size', 1],
    ['net_headers_buffer_size', 4096],
    ['net_input_buffer_size', 1],
    ['socket_port', 6251],
    ['timing', 1],
];

// Configuration file.
$confs = [];

// H header file.
$defines = [];
$structs = [];

// C file.
$init_configs = [];

// Loop through each line
foreach($map as $value) {
    // A section
    if(is_string($value)) {
        $comment = "// {$value}";
        // Configuration file.
        $confs[] = "\n# " . $value;

        // H file.
        $defines[] = "\n" . $comment;
        $structs[] = '    ' . $comment;

        // C file.
        $init_configs[] = '    ' . $comment;

        continue;
    }

    if(!is_array($value) || !count($value)) {
        continue;
    }

    $entry_name = $value[0];
    $entry_value = $value[1] ?? '';
    $enum_values = $value[2] ?? [];
    $is_integer = is_int($entry_value);
    $define_name = strtoupper($entry_name);
    $define = 'WRD_DEFAULT_' . $define_name;
    $type = $is_integer ? 'int ' : 'char *';
    $env_function = $is_integer ? 'wrd_get_env_int' : 'wrd_get_env_string';

    // Configuration file.
    $confs[] = "{$entry_name} = {$entry_value}";

    // An enum
    if(count($enum_values)) {
        $type = 'wrd_' . $entry_name;
        $defines[] = 'typedef enum {';
        $enums = [];

        foreach($enum_values as $enum_value) {
            $enums[] = '    WRD_' . strtoupper($entry_name) . '_' . strtoupper( $enum_value );
        }

        $defines[] = join(",\n", $enums);
        $defines[] = "} {$type};\n";
        $type .= ' ';
    }

    // H file.
    $defines[] = "#define {$define} " . ($is_integer ? $entry_value : '"' . $entry_value . '"');
    $structs[] = "    {$type}{$entry_name};";

    // C file.
    $init_configs[] = "    config->{$entry_name} = {$env_function}(\"WRD_{$define_name}\", {$define});";
}

$warning_message = '// This file automatically generated. Do not edit it manually.';

if ($argv[1] === 'h'): ?>
#ifndef WRD_CONF_H
#define WRD_CONF_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

<?php echo $warning_message, "\n"; ?>

#ifdef __cplusplus
extern "C" {
#endif
<?php echo join("\n", $defines), "\n"; ?>

typedef struct wrd_config {
<?php echo join("\n", $structs), "\n"; ?>
} wrd_config;

// Load a configuration file.
void wrd_init_config(wrd_config *config);

// Load a configuration file.
ssize_t wrd_load_config(wrd_config *config, const char *file_path);

#ifdef __cplusplus
}
#endif

#endif // WRD_CONF_H
<?php elseif ($argv[1] === 'c'): ?>

#include <ctype.h>

#include "conf.h"
#include "env.h"

<?php echo $warning_message, "\n"; ?>

// Init a configuration file with environment variables.
void wrd_init_config(wrd_config *config) {
<?php echo join("\n", $init_configs), "\n"; ?>
}

// Load a configuration file.
ssize_t wrd_load_config(wrd_config *config, const char *file_path) {
    wrd_init_config(config);

    if(file_path == NULL) {
        return 0;
    }

    FILE *stream = fopen(file_path, "r");

    if(stream == NULL) {
        return 0;
    }

    char *line = NULL;
    size_t length = 0;
    ssize_t nread = 0;
    /*enum statuses {
        INIT,
        NAME,
        EQUALS,
        VALUE,
    } status = INIT;*/

    unsigned int loaded = 0;

    #define is_char(c) isalpha(c) || isdigit(c) || c == '.' || c == '-' || c == '_'

    while((nread = getline(&line, &length, stream)) != -1) {
        // printf(">>%s<<%zd", line, nread);
        size_t start_name = 0;
        size_t start_value = 0;

        // Empty line
        if(nread <= 1 || (nread == 1 && line[0] == '\n')) {
            continue;
        }

        // Comment
        if(line[0] == '#') {
            continue;
        }

        for(ssize_t i = 0; i < nread; ++i) {
            if(line[i] == '=') {
                start_value = i + 1;
            }

            /*if(status == INIT) {
                if(line[i] == ' ' || line[i] == '\t') {
                    continue;
                } else if(line[i] == '#') {
                    break;
                } else if(is_char(line[i])) {
                    status = NAME;
                    start_name = i;
                }
            } else if(status == NAME) {
                if(is_char(line[i])) {
                    continue;
                } else if (line[i] == ' ' || line[i] == '\t') {
                    status = EQUALS;
                } else if (line[i] == '=') {
                    status = VALUE;
                } else {
                    break;
                }
            } else if(status == EQUALS) {
                if(line[i] == ' ' || line[i] == '\t') {
                    continue;
                } else if(line[i] == '=') {
                    status = VALUE;
                } else {
                    break;
                }
            } else if(status == VALUE) {
                if(line[i] == ' ' || line[i] == '\t') {
                    continue;
                } else {
                    start_value = i;
                }
            }*/
        }

        /*if(status == VALUE && start_value > 0) {
            ++loaded;
        }*/
    }

    free(line);
    fclose(stream);

    return loaded;
}
<?php elseif ($argv[1] === 'conf'): ?>
# This is the Warudo default configuration file.
# Lines starting with a '#' character are ignored as well as empty lines.

# A configuration is in the `name = value` form. `name=value` form is ok.
# The file must be UTF-8 encoded, line ending can be \n, \r\n. Multiple spaces
# at the end of a line are ignored.

# All configuration can also be passed as an environment variable to the
# executable. Example socket_port can be passed as WRD_SOCKET_PORT variable.
<?php echo join("\n", $confs); ?>

<?php endif;
