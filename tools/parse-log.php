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
$is_apache = false;

// Read the file line by line
while(($line = fgets($file)) !== false) {
    if($i === 0) {
        json_decode($line);

        // Check if the file is a list of JSON strings
        if(json_last_error() === JSON_ERROR_NONE) {
            $is_json = true;
        } else { // Check if the file is a list of Apache Combined Log Format strings
            $apache_log_pattern = '/^(\S+\s)?(\S+) (\S+) (\S+) \[([^\]]+)\] "([^"]+)" (\d+) (\d+) "([^"]+)" "([^"]+)"$/';

            if(preg_match($apache_log_pattern, $line)) {
                $is_apache = true;
            }
        }
    }

    ++$i;

    if($is_json) {
        echo $line;
        continue;
    } elseif($is_apache) {
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
}

// Close the file
fclose($file);
