#include "../src/conf.h"
#include "test.h"

void *test_conf(void *arg) {
    MOCK_CONFIG
    MOCK_CONFIG_S

    ATT_ASSERT(wrd_get_config_status(&config, WRD_DB_PATH), WRD_NOT_LOADED_CONFIG, "wrd_get_config_status not loaded")
    ATT_ASSERT(wrd_config_init(&config, NULL), WRD_DEFAULT, "NULL file path")

    WRD_MOCK_FILE("")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_DEFAULT, "Empty file")

    WRD_MOCK_FILE("\n")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Empty JSON")

    WRD_MOCK_FILE("{test")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_DB_ERROR, "Malformed JSON")

    WRD_MOCK_FILE("{}")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Valid JSON")

    WRD_MOCK_FILE("{ \"test\": true }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Valid JSON5")

    WRD_MOCK_FILE("{ // Test\ntest: 1, }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Valid JSON5 comment")

    WRD_MOCK_FILE("{ access_origin: 'a test' }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Valid access_origin")
    ATT_ASSERT(config.access_origin, "a test", "Valid access_origin")

    WRD_MOCK_FILE("{ net_buffer_size: 100 }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Add net_buffer_size")
    ATT_ASSERT(config.access_origin, NULL, "Access_origin NULL")
    ATT_ASSERT(config.net_buffer_size, 100, "Valid net_buffer_size")

    WRD_MOCK_FILE("{ net_buffer_size: '30' }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Add net_buffer_size")
    ATT_ASSERT(config.net_buffer_size, WRD_DEFAULT_NET_BUFFER_SIZE, "Default net_buffer_size")

    // Default
    ATT_ASSERT(wrd_get_config_status(&config, WRD_DB_PATH), WRD_DEFAULT_CONFIG, "Default DB path")

    // Loaded from config file.
    WRD_MOCK_FILE("{ db_path: \"Test\" }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Add access_origin")
    ATT_ASSERT(wrd_get_config_status(&config, WRD_DB_PATH), WRD_LOADED_CONFIG, "Loaded wrd_get_config_status")

    // Loaded from environment variable.
    setenv("WRD_DB_PATH", "Test 2", 1);
    WRD_MOCK_FILE("{}")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "Load empty file")
    ATT_ASSERT(wrd_get_config_status(&config, WRD_DB_PATH), WRD_ENV_CONFIG, "Get env")
    ATT_ASSERT(config.db_path, "Test 2", "Env valid value")
    unsetenv("WRD_DB_PATH");

    FREE_MOCK_FILE

    return NULL;
}
