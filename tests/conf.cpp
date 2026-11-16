/**
 * The Warudo web server
 *
 * This file is distributed under the MIT License. See LICENSE for details.
 */

#include "test.h"

ATT_TEST(conf) {
    MOCK_CONFIG
    MOCK_CONFIG_S

    ATT_ASSERT(wrd_get_config_status(&config, WRD_PID_FILE), WRD_DEFAULT_CONFIG, "wrd_get_config_status not loaded");
    ATT_ASSERT(wrd_config_init(&config, NULL), WRD_DEFAULT, "NULL file path");

    WRD_MOCK_FILE("")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_DEFAULT, "Empty file");

    WRD_MOCK_FILE("\n")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Empty JSON");

    WRD_MOCK_FILE("{test")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_INVALID_CONFIG, "Malformed JSON");

    WRD_MOCK_FILE("{}")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Valid JSON");

    WRD_MOCK_FILE("{ \"test\": true }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Valid JSON5");

    WRD_MOCK_FILE("{ // Test\ntest: 1, }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Valid JSON5 comment");

    WRD_MOCK_FILE("{ access_origin: 'a test' }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Valid access_origin");
    ATT_ASSERT(config.access_origin, "a test", "Valid access_origin");

    WRD_MOCK_FILE("{ net_buffer_size: 100 }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Add net_buffer_size");
    ATT_ASSERT(config.access_origin, NULL, "Access_origin NULL");
    ATT_ASSERT(config.net_buffer_size, 100, "Valid net_buffer_size");

    WRD_MOCK_FILE("{ net_buffer_size: '30' }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Add net_buffer_size");
    ATT_ASSERT(config.net_buffer_size, WRD_DEFAULT_NET_BUFFER_SIZE, "Default net_buffer_size");

    // Default
    ATT_ASSERT(wrd_get_config_status(&config, WRD_PID_FILE), WRD_DEFAULT_CONFIG, "Default pid_file status");
    ATT_ASSERT(config.pid_file, WRD_DEFAULT_PID_FILE, "Default pid_file");

    // Loaded from config file.
    WRD_MOCK_FILE("{ pid_file: \"Test\" }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Add pid_file");
    ATT_ASSERT(wrd_get_config_status(&config, WRD_PID_FILE), WRD_LOADED_CONFIG, "Loaded pid_file");
    ATT_ASSERT(config.pid_file, "Test", "Loaded pid_file value");

    // Loaded from environment variable.
    setenv("WRD_PID_FILE", "Test 2", 1);
    WRD_MOCK_FILE("{}")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Load empty file");
    ATT_ASSERT(wrd_get_config_status(&config, WRD_PID_FILE), WRD_ENV_CONFIG, "Get env");
    ATT_ASSERT(config.pid_file, "Test 2", "Env valid value");
    unsetenv("WRD_PID_FILE");

    // Only comments and white space.
    WRD_MOCK_FILE("// Nothing here\n/* at all */\n")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Only comments");

    // The configurations must be in an object.
    WRD_MOCK_FILE("[1, 2]")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_INVALID_CONFIG, "Array document");

    WRD_MOCK_FILE("{ port: 8080, } trailing")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_INVALID_CONFIG, "Trailing content");

    // Integers.
    WRD_MOCK_FILE("{ port: 0x1F90, tls_enabled: true, hsts_max_age: 1.5, tls_port: 99999999999 }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Integer forms");
    ATT_ASSERT(config.port, 8080, "Hexadecimal port");
    ATT_ASSERT(wrd_get_config_status(&config, WRD_PORT), WRD_LOADED_CONFIG, "Loaded port");
    ATT_ASSERT(config.tls_enabled, 1, "Boolean as integer");
    ATT_ASSERT(config.hsts_max_age, WRD_DEFAULT_HSTS_MAX_AGE, "Fraction is not an integer");
    ATT_ASSERT(wrd_get_config_status(&config, WRD_HSTS_MAX_AGE), WRD_DEFAULT_CONFIG, "Default hsts_max_age");
    ATT_ASSERT(config.tls_port, WRD_DEFAULT_TLS_PORT, "Out of range integer");

    // Strings.
    WRD_MOCK_FILE("{ worker_processes: 4, document_root: '', access_origin: null, pid_file: 'a\\u00E8\\\nb' }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "String forms");
    ATT_ASSERT(config.worker_processes, "4", "Number as string");
    ATT_ASSERT(config.document_root, NULL, "Empty string keeps the default");
    ATT_ASSERT(config.access_origin, NULL, "null keeps the default");
    ATT_ASSERT(config.pid_file, "a\xC3\xA8" "b", "Escapes and line continuation");

    // The last duplicated key wins.
    WRD_MOCK_FILE("{ port: 1, port: 2 }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Duplicated key");
    ATT_ASSERT(config.port, 2, "Last duplicated key");

    // The default configuration file is valid.
    ATT_ASSERT(wrd_config_init(&config, WRD_TEST_SOURCE_DIR "/warudo.conf.default"), WRD_OK, "Default file");
    ATT_ASSERT(config.port, WRD_DEFAULT_PORT, "Default file port");
    ATT_ASSERT(wrd_get_config_status(&config, WRD_PORT), WRD_LOADED_CONFIG, "Default file port status");

    FREE_MOCK_FILE
}
