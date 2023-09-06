<?php

// Open the log file, or stdin for reading
$file = fopen($argc < 2 || $argv[1] === '-' ? 'php://stdin' : $argv[1], 'r');

// Check if the file was opened successfully
if(!$file) {
    echo "Failed to open the log file.\n";
    exit(1);
}

$i = 0;
$is_json = false;

// Read the file line by line
while(($line = fgets($file)) !== false) {
    if($i === 0) {
        json_decode($line);

        // Check if the file is a list of JSON strings
        $is_json = json_last_error() === JSON_ERROR_NONE;
    }

    ++$i;

    if($is_json) {
        echo $line;
        continue;
    }

    // Scan the line
    $log = [];
    sscanf($line, '%s %s %s %s [%[^]]] "%s %s %[^"]" %d %d "%[^"]" "%[^"]"',
        $log['url'],
        $log['ip'],
        $log['client'],
        $log['user'],
        $log['time'],
        $log['method'],
        $log['uri'],
        $log['protocol'],
        $log['status_code'],
        $log['bytes'],
        $log['ref'],
        $log['agent']
    );

    if($log['time']) {
        $log['time'] = date('c', strtotime($log['time']));
    }

    unset($log['ref']);
    unset($log['agent']);
    echo json_encode($log), "\n";
}

// Close the file
fclose($file);
