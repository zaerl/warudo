<?php

namespace J6251;

use SQLite3;
use Exception;
use stdClass;

define('J6251_VERSION', '0.1.0');

$settings_path = getenv('J6251_CONFIG_PATH') ?: './config.ini';
$config = (object)parse_ini_file($settings_path);
$database = null;

$config->columns = [
    'url' => ['type' => 'text', 'db' => SQLITE3_TEXT, 'required' => true],
    'ip' => ['type' => 'text', 'db' => SQLITE3_TEXT, 'required' => true],
    'client' => ['type' => 'text', 'db' => SQLITE3_TEXT, 'required' => false],
    'user' => ['type' => 'text', 'db' => SQLITE3_TEXT, 'required' => false],
    'time' => ['type' => 'text', 'db' => SQLITE3_TEXT, 'required' => true],
    'method' => ['type' => 'text', 'db' => SQLITE3_TEXT, 'required' => true],
    'uri' => ['type' => 'text', 'db' => SQLITE3_TEXT, 'required' => true],
    'protocol' => ['type' => 'text', 'db' => SQLITE3_TEXT, 'required' => true],
    'status_code' => ['type' => 'integer', 'db' => SQLITE3_INTEGER, 'required' => true],
    'bytes' => ['type' => 'integer', 'db' => SQLITE3_INTEGER, 'required' => true],
    // 'ref' => ['type' => 'text', 'db' => SQLITE3_TEXT, 'required' => false],
    // 'agent' => ['type' => 'text', 'db' => SQLITE3_TEXT, 'required' => false],
];

// Check if OPCache is enabled
if (!function_exists('opcache_get_status') || !opcache_get_status()['opcache_enabled']) {
    output_data('OPCache is disabled. Please enable it to use this application.', $config, 500);
}

try {
    $database = open_database($config->db_path, $config);
} catch(Exception $ex) {
    output_data($ex, $config, 500);
}

header('Content-Type: application/json; charset=utf-8');

try {
    $method = strtoupper($_SERVER['REQUEST_METHOD']);

    if ($method === 'POST') {
        add_entry($database, $config);
    } elseif ($method === 'DELETE') {
        delete_entry($database, $config);
    } else {
        output_entries($database, $config);
    }
} catch(Exception $ex) {
    output_data($ex, $config, 500);
}

close_database($database);

function add_entry(SQLite3 $db, stdClass $config): void {
    $content_type = $_SERVER['CONTENT_TYPE'] ?? 'application/json';
    $data = file_get_contents('php://input');
    $entries = [];

    if (strpos($content_type, 'application/json') !== false ||
        strpos($content_type, 'text/plain') !== false) {
        $data = json_decode($data);

        if(!$data) {
            output_data('Invalid data.', $config, 400);

            return;
        }

        if(!property_exists($data, 'entries') || !is_array($data->entries)) {
            output_data("Invalid entries.", $config, 400);

            return;
        }

        $entries = $data->entries;
    } else {
        $entries[] = (object)$_POST;
    }

    $columns = implode(',', array_keys($config->columns));
    $values = [];

    foreach($config->columns as $key => $value) {
        $values[] = ':' . $key;
    }

    $values = implode(',', $values);

    // Prepare the INSERT statement
    $query = "INSERT INTO entries ({$columns}) VALUES ({$values})";
    $stmt = $db->prepare($query);
    $output = new stdClass();
    $output->entries = [];
    $error = false;

    foreach($entries as $entry) {
        $valid = true;
        $inserted = new stdClass();
        $inserted->id = null;

        foreach($config->columns as $key => $value) {
            $stmt->bindValue(':' . $key, $entry->{$key}, $value['db']);
            $valid_param = valid_parameter($entry, $key, $config);

            if(!$valid_param->ok) {
                $error = true;
                $valid = false;
                $inserted->error = $valid_param->error;
                $output->entries[] = $inserted;
                break;
            }
        }

        if(!$valid) {
            continue;
        }

        if($stmt->execute()) {
            $inserted->id = $db->lastInsertRowID();
        } else {
            $error = true;
        }

        $output->entries[] = $inserted;
    }

    output_data($output, $config, $error ? 500 : 200);
}

function valid_parameter(stdClass $data, string $name, $config): stdClass {
    $column = $config->columns[$name];
    $ret = new stdClass();
    $ret->ok = true;

    if($column['required'] && !property_exists($data, $name)) {
        $ret->ok = false;
        $ret->error = "Missing {$name}.";

        return false;
    }

    switch($column['type']) {
        case 'text':
            if(!is_string($data->{$name})) {
                $ret->ok = false;
                $ret->error = "Invalid string {$name}.";
            }

            break;
        case 'integer':
            if(!is_int($data->{$name})) {
                $ret->ok = false;
                $ret->error = "Invalid integer {$name}.";
            }

            break;
    }

    return $ret;
}

function delete_entry(SQLite3 $db, stdClass $config): void {
    $id = (int)$_GET['id'];

    if(!$id || $id < 1) {
        output_data('Invalid ID.', $config, 400);

        return;
    }

    $query = "DELETE FROM entries WHERE id = :id";
    $stmt = $db->prepare($query);

    $stmt->bindValue(':id', $id, SQLITE3_INTEGER);
    $stmt->execute();

    $rows = $db->changes();

    if($rows === 0) {
        output_data('The ID provided do not exists.', $config, 404);

        return;
    }

    output_data("$rows removed.", $config);
}

function output_entries(SQLite3 $db, stdClass $config) {
    $limit = (int)($_GET['limit'] ?? $config->default_limit);
    $page = (int)($_GET['page'] ?? 1);

    if(!$limit || $limit <= 0 || $limit > $config->default_limit) {
        output_data("Invalid limit (1-$config->default_limit).", $config, 400);

        return;
    }

    if(!$page || $page <= 0) {
        output_data("Invalid page (1-N).", $config, 400);

        return;
    }

    // Count
    $query = "SELECT COUNT(*) FROM entries";
    $count = $db->querySingle($query);

    if($count !== false) {
        $pages = ceil($count / $limit);

        set_custom_header('count', $count);
        set_custom_header('pages', $pages);
    }

    // Query the database
    $query = "SELECT * FROM entries LIMIT :limit OFFSET :offset";
    $stmt = $db->prepare($query);
    $stmt->bindValue(':limit', $limit, SQLITE3_INTEGER);
    $stmt->bindValue(':offset', ($page - 1) * $limit, SQLITE3_INTEGER);
    $result = $stmt->execute();

    // $result = $db->query($query);
    $output = new stdClass();
    $output->entries = [];

    // Fetch and display the data
    while ($row = $result->fetchArray(SQLITE3_ASSOC)) {
        $output->entries[] = $row;
    }

    set_custom_header('page', $page);
    set_custom_header('limit', $limit);

    output_data($output, $config);
}

function output_data(stdClass | Exception | string $data, stdClass $config, int $status = 200) {
    $output = new stdClass();
    set_http_status($status);

    if($config->show_version) {
        set_custom_header('version', J6251_VERSION);
    }

    if ($data instanceof stdClass) {
        $output = $data;
    } elseif($data instanceof Exception) {
        $output->error = $data->getMessage();
    } else if(is_string($data)) {
        $output->error = $data;
    }

    echo json_encode($output, $config->verbose ? JSON_PRETTY_PRINT : 0);
}

function set_http_status(int $status): void {
    $codes = [
        200 => 'OK',
        201 => 'Created',
        400 => 'Bad Request',
        404 => 'Not Found',
        500 => 'Internal Server Error',
    ];

    if (isset($codes[$status])) {
        $text = $codes[$status];
        $protocol = isset($_SERVER['SERVER_PROTOCOL']) ? $_SERVER['SERVER_PROTOCOL'] : 'HTTP/1.1';
        header("$protocol $status $text");
    } else {
        // Invalid status code provided
        header('HTTP/1.1 500 Internal Server Error');
    }
}

function set_custom_header(string $name, string|int $value) {
    header("X-J6251-{$name}: {$value}");
}

function open_database(string $path, $config): SQLite3 {
    try {
        $db = new SQLite3($path);
        $db->enableExceptions(true);
    } catch(Exception $ex) {
        output_data($ex, $config, 500);
    }

    $columns = ['id INTEGER PRIMARY KEY AUTOINCREMENT'];

    foreach($config->columns as $key => $value) {
        $column = "{$key} {$value['type']}";

        if($value['required']) {
            $column .= ' NOT NULL';
        }

        $columns[] = $column;
    }

    $query = "CREATE TABLE IF NOT EXISTS entries (" . implode(', ', $columns) . ")";

    $db->exec($query);

    return $db;
}

function close_database(SQLite3 $db): void {
    $db->close();
}
