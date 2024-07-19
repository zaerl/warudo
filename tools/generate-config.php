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

// List of configurations.
$map = [
    'Database',
    ['db_path', 'file:warudo.db'],
    'Log level can be one of the following [0, 1, 2, 3]: no_log, info, error, debug',
    ['log_level', 0, ['no_log', 'info', 'error', 'debug']],
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
$structs = [];

// C file.
$init_configs = [];

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

function values_to_struct($values) {
    return array_map(function($value) { return '    ' . $value; }, $values);
}

function inject_text($file, $start_text, $end_text, $text, $start_pos = 0) {
    $start_pos = strpos($file, $start_text, $start_pos);

    if($start_pos === false) {
        echo "Could not find the start of injection point.\n";
        exit(1);
    }

    $start_pos += strlen($start_text) + 1;
    $end_pos = strpos($file, $end_text, $start_pos);

    if($end_pos === false) {
        echo "Could not find the end of the injection point.\n";
        exit(1);
    }

    return [
        'start_pos' => $start_pos,
        'text' => substr_replace($file, $text, $start_pos, $end_pos - $start_pos),
    ];
}

// Loop through each line
foreach($map as $value) {
    // A section
    if(is_string($value)) {
        $comment = "// {$value}";
        // Configuration file.
        $confs[] = "// " . $value;

        // H file.
        $defines[] = "\n" . $comment;
        $structs[] = $comment;

        // C file.
        $init_configs[] = $comment;

        continue;
    }

    if(!is_array($value) || !count($value)) {
        continue;
    }

    $entry_name = $value[0];
    $entry_value = $value[1];
    $enum_values = $value[2] ?? [];
    $is_integer = is_int($entry_value);
    $define_name = strtoupper($entry_name);
    $define = 'WRD_DEFAULT_' . $define_name;
    $type = $is_integer ? 'int ' : 'char *';
    $env_function = $is_integer ? 'wrd_get_env_int' : 'wrd_get_env_string';
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

        $defines[] = join(",\n", values_to_struct($enums));
        $defines[] = "} {$type};\n";
        $type .= ' ';
    }

    $c_entry_value = value_to_c($entry_value);

    // H file.
    $defines[] = "#define {$define} " . $c_entry_value;
    $structs[] = "{$type}{$entry_name};";

    // C file.
    $init_configs[] = "{$env_function}({$env_cast}&config->{$entry_name}, \"WRD_{$define_name}\", {$define});";
}

$warning_message = '// This file automatically generated. Do not edit it manually.';

if ($argv[1] === 'h') {
    $warudo_h = file_get_contents('src/warudo.h');
    $start_comment = '// Configurations.';
    $injection = inject_text($warudo_h, $start_comment, "\n#include <", join("\n", $defines) . "\n");
    $injection = inject_text($injection['text'], $start_comment, "\n\n", "\n" . join("\n", values_to_struct($structs)), $injection['start_pos']);

    if(file_put_contents('src/warudo.h', $injection['text']) === false) {
        echo "Could not write to src/warudo.h.\n";
        exit(1);
    }
} elseif ($argv[1] === 'c') {
    $conf_c = file_get_contents('src/conf.c');
    $start = 'WRD_API void wrd_init_config(warudo *config) {';
    $injection = inject_text($conf_c, $start, "\n}", join("\n", values_to_struct($init_configs)));

    if(file_put_contents('src/conf.c', $injection['text']) === false) {
        echo "Could not write to src/conf.c.\n";
        exit(1);
    }
} elseif ($argv[1] === 'conf') {
    $conf = file_get_contents('warudo.conf.default');
    $start = '{';
    $injection = inject_text($conf, $start, "}", join("\n", values_to_struct($confs)) . "\n");

    if(file_put_contents('warudo.conf.default', $injection['text']) === false) {
        echo "Could not write to warudo.conf.default.\n";
        exit(1);
    }
}
