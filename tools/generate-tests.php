<?php
// phpcs:ignoreFile

if(php_sapi_name() !== 'cli') {
    echo 'This script can only be run from the command line.';
    exit(1);
}

$c_files = glob('src/*.c');
$c_files = array_merge($c_files, glob('cli/*.c'));

// main.c don't need to be tested.
$key = array_search("src/main.c", $c_files);

if ($key !== false) {
    unset($c_files[$key]);
}

// main.c don't need to be tested.
$key = array_search("cli/main.c", $c_files);

if ($key !== false) {
    unset($c_files[$key]);
}

$names = array_map(function($filename) {
    return pathinfo($filename, PATHINFO_FILENAME);
}, $c_files);
sort($names);

$functs = array_map(function($name) {
    return 'void *test_' . str_replace('-', '_', $name) . '(void *arg)';
}, $names);

$run_macro = array_map(function($name) {
    return '    RUN_TEST(' . str_replace('-', '_', $name) . ')';
}, $names);

foreach($names as $i => $filename) {
    $filename = basename($filename);
    $dest_filename = 'test/' . $filename . '.c';

    if(!file_exists($dest_filename)) {
        $new_file = "#include \"test.h\"\n\n{$functs[$i]} {\n    return NULL;\n}\n";
        file_put_contents($dest_filename, $new_file);

        echo "Created {$dest_filename}\n";
    }
}

$test_h = file_get_contents('test/test.h');

if($test_h === false) {
    echo "Could not open test/test.h.\n";
    exit(1);
}

// Modify test/test.h
$start_comment = '// Start tests declarations.';
$start_pos = strpos($test_h, $start_comment);

if($start_pos === false) {
    echo "Could not find the start of the tests declarations.\n";
    exit(1);
}

$start_pos += strlen($start_comment) + 1;
$end_pos = strpos($test_h, "\n\n", $start_pos);

if($end_pos === false) {
    echo "Could not find the end of the tests declarations.\n";
    exit(1);
}

$new_file_h = substr_replace($test_h, implode(";\n", $functs) . ";", $start_pos, $end_pos - $start_pos);

if(file_put_contents('test/test.h', $new_file_h) === false) {
    echo "Could not write to test/test.h.\n";
    exit(1);
}

// Modify test/test.c
$test_c = file_get_contents('test/test.c');

if($test_c === false) {
    echo "Could not open test/test.c.\n";
    exit(1);
}

$start_pos = strpos($test_c, $start_comment);

if($start_pos === false) {
    echo "Could not find the start of the tests declarations.\n";
    exit(1);
}

$start_pos += strlen($start_comment) + 1;
$end_pos = strpos($test_c, "\n\n", $start_pos);

if($end_pos === false) {
    echo "Could not find the end of the tests declarations.\n";
    exit(1);
}

$new_file_h = substr_replace($test_c, implode("\n", $run_macro), $start_pos, $end_pos - $start_pos);

if(file_put_contents('test/test.c', $new_file_h) === false) {
    echo "Could not write to test/test.c.\n";
    exit(1);
}

// Modify CMakeLists.txt
$cmake = file_get_contents('test/CMakeLists.txt');

if($cmake === false) {
    echo "Could not open test/CMakeLists.txt.\n";
    exit(1);
}

$start_line = "attractor/attractor.c\n    ";
$start_pos = strpos($cmake, $start_line);

if($start_pos === false) {
    echo "Could not find the start of the CMakeLists files.\n";
    exit(1);
}

$start_pos += strlen($start_line);
$end_pos = strpos($cmake, ")\n", $start_pos);

if($end_pos === false) {
    echo "Could not find the end of the CMakeLists files.\n";
    exit(1);
}

$names[] = 'test';
$names[] = 'test-utils';
sort($names);
$new_cmake = substr_replace($cmake, implode(".c\n    ", $names) . ".c\n", $start_pos, $end_pos - $start_pos);

if(file_put_contents('test/CMakeLists.txt', $new_cmake) === false) {
    echo "Could not write to test/CMakeLists.txt.\n";
    exit(1);
}
