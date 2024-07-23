<?php
// phpcs:ignoreFile

if(php_sapi_name() !== 'cli') {
    echo 'This script can only be run from the command line.';
    exit(1);
}

if($argc < 2) {
    echo "Usage: php generate-config.php h|c|conf\n";
    exit(1);
}

for($i = 1; $i < $argc; ++$i) {
    if(!in_array($argv[1], ['h', 'c', 'conf', 'server'])) {
        echo "Wrong file type. Accepted files: h|c|conf|server\n";
        exit(1);
    }
}

// List of configurations.
$map = [
    'Database',
    ['db_path', 'file:warudo.db'],
    ['log_level', 0, ['no_log', 'info', 'error', 'debug'], 'Log level can be one of the following [0, 1, 2, 3]: no_log, info, error, debug'],
    'Net',
    ['access_origin', null],
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
$names = [];
$statuses = [];
$structs = [];

// C file.
$init_configs = [];
$free_configs = [];
$db_loads = [];
$env_loads = [];

// Server file.
$logs = [];

$struct_count = count(array_filter($map, 'is_array'));
$statuses[] = "// Configuration statuses.";
$structs[] = "char config_status[{$struct_count}];";
$structs[] = '';
$init_configs[] = '// Initialize to defaults.';
$init_configs[] = "memset(config->config_status, WRD_DEFAULT_CONFIG, {$struct_count});\n";

// Loop through each line
foreach($map as $value) {
    $comment = null;

    if(is_string($value)) {
        $comment = $value;
    }

    // A section
    if(is_string($value)) {
        $comment = "// {$value}";
        // Configuration file.
        $confs[] = $comment;

        // H file.
        $defines[] = "\n" . $comment;
        $structs[] = $comment;

        // C file.
        $init_configs[] = $comment;
        $free_configs[] = $comment;
        $env_loads[] = $comment;

        continue;
    }

    if(!is_array($value) || !count($value)) {
        continue;
    }

    if(count($value) === 4) {
        $confs[] = '// ' . $value[3];
    }

    $entry_name = $value[0];
    $entry_value = $value[1];
    $enum_values = $value[2] ?? [];
    $is_integer = is_int($entry_value);
    $define_name = strtoupper($entry_name);
    $define = 'WRD_DEFAULT_' . $define_name;
    $type = $is_integer ? 'int ' : 'char *';
    $env_function = $is_integer ? 'LOAD_ENV_CONFIG_INT' : 'LOAD_ENV_CONFIG_STRING';
    $env_cast = $is_integer ? '(int*)' : '';

    // Configuration file.
    $conf_entry_value = value_to_conf($entry_value);
    $confs[] = "{$entry_name}: {$conf_entry_value},";

    // An enum
    if(count($enum_values)) {
        $type = 'wrd_' . $entry_name;
        $defines[] = 'typedef enum {';
        $enums = [];

        foreach($enum_values as $enum_value) {
            $enums[] = 'WRD_' . strtoupper($entry_name) . '_' . strtoupper( $enum_value );
        }

        $defines[] = join(",\n", add_indentation($enums));
        $defines[] = "} {$type};\n";
        $type .= ' ';
    }

    $c_entry_value = value_to_c($entry_value);

    // H file.
    $defines[] = "#define {$define} " . $c_entry_value;
    $structs[] = "{$type}{$entry_name};";
    $names[] = "WRD_{$define_name},";

    // C file.
    $init_configs[] = "config->{$entry_name} = {$define};";
    $config_name = explode('_', $entry_name);
    $config_name = array_map('ucfirst', $config_name);
    $config_name = join(' ', $config_name);

    if($is_integer) {
        $db_loads[] = "LOAD_DB_CONFIG_INT(WRD_{$define_name}, {$entry_name})";
        $logs[] = "wrd_log_info(config, u8\"{$config_name}: %d [%c]\\n\", config->{$entry_name}, wrd_get_config_status(config, WRD_{$define_name}));";
    } else {
        $db_loads[] = "LOAD_DB_CONFIG_STR(WRD_{$define_name}, {$entry_name})";
        $logs[] = "wrd_log_info(config, u8\"{$config_name}: %s [%c]\\n\", config->{$entry_name}, wrd_get_config_status(config, WRD_{$define_name}));";
    }

    if(!$is_integer) {
        // $free_configs[] = "if(config->{$entry_name}) {\n        free(config->{$entry_name});\n        config->{$entry_name} = NULL;\n    }\n";
        $free_configs[] = "FREE_CONFIG_STRING(WRD_{$define_name}, $entry_name)\n";
    }

    $env_loads[] = "{$env_function}(WRD_{$define_name}, {$entry_name})";
}

$defines = array_merge(
    [
        '// Configuration names.',
        'typedef enum {'
    ],
    add_indentation($names),
    ['} wrd_config_name;'],
    $defines
);

$files = [
    'h' => [
        'file' => 'src/warudo.h',
        'start' => '// Configurations.',
        'end' => "\n#include <",
        'text' => join("\n", $defines) . "\n",
        'additionals' => [
            [
                'end' => "\n    // Network",
                'text' => "\n" . join("\n", add_indentation($structs)) . "\n",
            ],
        ]
    ],
    'c' => [
        'file' => 'src/conf.c',
        'start' => '// Configurations.',
        'end' => "\n    return WRD_OK;",
        'text' => "\n" . join("\n", add_indentation($init_configs)) . "\n",
        'additionals' => [
            [
                'end' => "\n    return WRD_OK;",
                'text' => "\n" . join("\n", add_indentation($env_loads)) . "\n",
            ], [
                'end' => "\n    return WRD_OK;",
                'text' => "\n" . join("\n", add_indentation($free_configs)),
            ], [
                'end' => "\n    wrd_load_config_env",
                'text' => "\n" . join("\n", add_indentation($db_loads)) . "\n",
            ]
        ]
    ],
    'conf' => [
        'file' => 'warudo.conf.default',
        'start' => '{',
        'end' => '}',
        'text' => join("\n", add_indentation($confs)) . "\n",
        'additionals' => [],
    ],
    'server' => [
        'file' => 'src/server.c',
        'start' => '// Configurations.',
        'end' => "res = wrd_db_init(config->db_path, config);",
        'text' => join("\n", add_indentation($logs)) . "\n\n    ",
        'additionals' => [],
    ],
];

for($i = 1; $i < $argc; ++$i) {
    $type = $argv[$i];
    $file = $files[$type];
    $content = file_get_contents($file['file']);
    $injection = inject_text($content, $file['start'], $file['end'], $file['text']);

    foreach($file['additionals'] as $additional) {
        $injection = inject_text($injection['text'], $file['start'], $additional['end'], $additional['text'], $injection['start_pos']);
    }

    if(file_put_contents($file['file'], $injection['text']) === false) {
        echo "Could not write to {$file['file']}.\n";
        exit(1);
    }

    echo "Generated {$file['file']}.\n";
}

// Transform a value to a configuration file value.
function value_to_conf($value) {
    if(is_null($value)) {
        return 'null';
    }

    if(is_int($value)) {
        return $value;
    }

    return '\'' . str_replace('\'', '\\\'', $value) . '\'';
}

// Transform a value to a C file value.
function value_to_c($value) {
    if(is_null($value)) {
        return 'NULL';
    }

    if(is_int($value)) {
        return $value;
    }

    return '"' . str_replace('"', '\\"', $value) . '"';
}

// Add indentation to an array of values.
function add_indentation($values, $indentation = 1) {
    $indentation = str_repeat(' ', $indentation * 4);

    return array_map(
        function($value) use ($indentation) {
            return $indentation . $value;
        },
        $values
    );
}

function inject_text($file, $start_text, $end_text, $text, $start_pos = 0) {
    $start_pos = strpos($file, $start_text, $start_pos);

    if($start_pos === false) {
        echo "Could not find the start of injection point ('{$start_text}').\n";
        exit(1);
    }

    $start_pos += strlen($start_text) + 1;
    $end_pos = strpos($file, $end_text, $start_pos);

    if($end_pos === false) {
        echo "Could not find the end of the injection point ('{$end_text}').\n";
        exit(1);
    }

    return [
        'start_pos' => $start_pos,
        'text' => substr_replace($file, $text, $start_pos, $end_pos - $start_pos),
    ];
}
