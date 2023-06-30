<?php

if ($argc < 2) {
    echo "Please provide the path to access log file as a command line argument.\n";
    exit(1);
}

// Get the log file path from the command line argument
$path = $argv[1];
$url = 'http://localhost:6251';
$curl = curl_init($url);
curl_setopt($curl, CURLOPT_POST, true);
curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);

// Open the log file for reading
$file = fopen($path, 'r');

// Check if the file was opened successfully
if (!$file) {
    echo "Failed to open the log file.\n";
    exit(1);
}

$buffer = [];
$limit = 100;

// Read the file line by line
while (($line = fgets($file)) !== false) {
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

    $time = date('c', strtotime($log['time']));
    $log['time'] = $time;

    unset($log['ref']);
    unset($log['agent']);

    $buffer[] = $log;

    if(count($buffer) === $limit) {
        post_data($buffer, $curl);
        $buffer = [];

        continue;
    }
}

if(count($buffer)) {
    post_data($buffer, $curl);
}

function post_data($buffer, $curl) {
    $post = new stdClass();
    $post->entries = $buffer;
    $json = json_encode($post);

    // Set the request headers
    curl_setopt($curl, CURLOPT_HTTPHEADER, [
        'Content-Type: application/json',
        'Content-Length: ' . strlen($json)
    ]);

    // Set the POST data
    curl_setopt($curl, CURLOPT_POSTFIELDS, $json);

    // Execute the request
    $response = curl_exec($curl);

    // Check for errors
    if ($response === false) {
        echo "Error: " . curl_error($curl), "\n";
    } else {
        $response = json_decode($response);
        echo '.';
    }
}

echo "\n";

// Close the file
fclose($file);
