<?php
// phpcs:ignoreFile

function run_process($command) {
    $descriptorspec = [
        0 => ['pipe', 'r'],
        1 => ['pipe', 'w'],
        2 => ['pipe', 'w'],
     ];

    $process = proc_open($command, $descriptorspec, $pipes);

    if(is_resource($process)) {
        fclose($pipes[0]);

        $output = stream_get_contents($pipes[1]);
        fclose($pipes[1]);

        $return_value = proc_close($process);

        if($return_value !== 0) {
            return $return_value;
        } else {
            return $output;
        }
    } else {
        return 1;
    }

    return 2;
}

function analyze_output($output, $verbose = false) {
    $lines = explode("\n", $output);
    $pattern = '/\b(\w+)\s+\*?\b(\w+)\s*\(/';
    $coverage = [];

    foreach($lines as $i => $line) {
        if(
            $line === '' ||
            strpos($line, 'Match #') === 0 ||
            strpos($line, 'Binding for') === 0 ||
            $i === count($lines) - 2
        ) {
            continue;
        } else {
            if (preg_match($pattern, $line, $matches)) {
                if($verbose) {
                    echo $line, "\n    > ", $matches[2], "\n";
                }

                $coverage[$matches[2]] = 0;
            } else {
                if($verbose) {
                    echo $line . "\n";
                }
            }
        }
    }

    return $coverage;
}

function analyze_test($output, $coverage, $verbose = false) {
    $lines = explode("\n", $output);
    // $pattern = '/\_Generic\(0 , (.+)\), char: att_assert_c.+/';
    $pattern = '/\_Generic\(\(0\s,\s(.+)\),\schar: att_assert_c/';

    foreach($lines as $i => $line) {
        if(
            $line === '' ||
            strpos($line, 'Match #') === 0 ||
            strpos($line, 'Binding for') === 0 ||
            $i === count($lines) - 2
        ) {
            continue;
        } else {
            if(preg_match($pattern, $line, $matches)) {
                if($verbose) {
                    echo $matches[1];
                }

                if(preg_match('/([a-z0-9_]+)\(.*\)/', $matches[1], $matches_2)) {
                    if($verbose) {
                        echo "\n    > ", $matches_2[1], "\n";
                    }

                    if(!isset($coverage[$matches_2[1]])) {
                        $coverage[$matches_2[1]] = 0;
                    }

                    ++$coverage[$matches_2[1]];
                }
            }
        }
    }

    return $coverage;
}

$output = run_process('clang-query -f tools/src-query.txt ./src/*.h');

if(is_int($output)) {
    echo "Source command returned $output\n";

    exit($output);
}

$coverage = analyze_output($output);

$output = run_process('clang-query -f tools/test-query.txt ./test/*.c');

if(is_int($output)) {
    echo "Test command returned $output\n";

    exit($output);
}

$coverage = analyze_test($output, $coverage);
$total = 0;
$max_name_length = 0;

foreach($coverage as $key => $value) {
    $total += $value;
    $max_name_length = max($max_name_length, strlen($key));
}

uksort($coverage, function($a, $b) use ($coverage) {
    if(
        // true true
        // false false
        $coverage[$a] && $coverage[$b] ||
        !$coverage[$a] && !$coverage[$b]
    ) {
        return strcmp($a, $b);
    }

    // true false
    if($coverage[$a] && !$coverage[$b]) {
        return -1;
    }

    // false true
    if(!$coverage[$a] && $coverage[$b]) {
        return 1;
    }
});
$bar = str_repeat('-', $max_name_length);
$post = str_repeat(' ', $max_name_length - 5);
$post_2 = str_repeat(' ', 5);
?>
# Test coverage

| Test <?php echo $post ?> | Coverage |
| <?php echo $bar ?> | -------- |
<?php
foreach($coverage as $key => $value) {
    $post = str_repeat(' ', $max_name_length - strlen($key));
    // echo $key, ' ', $value, "\n";
    echo '| ', $key, $post, ' | ', $value;

    $post = str_repeat(' ', 8 - strlen((string)$value));
    echo $post, " |\n";
}

$post = str_repeat(' ', $max_name_length - 6);
echo '| Total ', $post, ' | ', $total;

$post = str_repeat(' ', 9 - strlen((string)$total));
echo $post, "|\n";
