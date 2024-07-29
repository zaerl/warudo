#include "../src/conf.h"
#include "test.h"

void *test_conf(void *arg) {
    MOCK_CONFIG
    MOCK_CONFIG_S

    ATT_ASSERT(wrd_get_config_status(&config, WRD_DB_PATH), WRD_NOT_LOADED_CONFIG, "wrd_get_config_status not loaded")
    ATT_ASSERT(wrd_config_init(&config, NULL), WRD_DEFAULT, "wrd_config_init NULL")

    WRD_MOCK_FILE("")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_DEFAULT, "wrd_config_init empty")

    WRD_MOCK_FILE("\n")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "wrd_config_init empty JSON")

    WRD_MOCK_FILE("{test")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_DB_ERROR, "wrd_config_init malformed JSON")

    WRD_MOCK_FILE("{}")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "wrd_config_init valid JSON")

    WRD_MOCK_FILE("{ \"test\": true }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "wrd_config_init valid JSON")

    WRD_MOCK_FILE("{ // Test\ntest: 1, }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "wrd_config_init valid JSON5")

    WRD_MOCK_FILE("{ access_origin: 'a test' }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "wrd_config_init add access_origin")
    ATT_ASSERT(config.access_origin, "a test", "wrd_config_init valid access_origin")

    WRD_MOCK_FILE("{ net_buffer_size: 100 }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "wrd_config_init add net_buffer_size")
    ATT_ASSERT(config.access_origin, NULL, "wrd_config_init access_origin NULL")
    ATT_ASSERT(config.net_buffer_size, 100, "wrd_config_init valid net_buffer_size")

    WRD_MOCK_FILE("{ net_buffer_size: '30' }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "wrd_config_init add net_buffer_size")
    ATT_ASSERT(config.net_buffer_size, WRD_DEFAULT_NET_BUFFER_SIZE, "wrd_config_init default net_buffer_size")

    // Default
    ATT_ASSERT(wrd_get_config_status(&config, WRD_DB_PATH), WRD_DEFAULT_CONFIG, "wrd_get_config_status default")

    // Loaded from config file.
    WRD_MOCK_FILE("{ db_path: \"Test\" }")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "wrd_config_init add access_origin")
    ATT_ASSERT(wrd_get_config_status(&config, WRD_DB_PATH), WRD_LOADED_CONFIG, "wrd_get_config_status loaded")

    // Loaded from environment variable.
    setenv("WRD_DB_PATH", "Test 2", 1);
    WRD_MOCK_FILE("{}")
    ATT_ASSERT(wrd_config_init(&config, tmp_mock_file), WRD_OK, "wrd_config_init load empty file")
    ATT_ASSERT(wrd_get_config_status(&config, WRD_DB_PATH), WRD_ENV_CONFIG, "wrd_get_config_status env")
    ATT_ASSERT(config.db_path, "Test 2", "wrd_get_config_status env value")
    unsetenv("WRD_DB_PATH");

    FREE_MOCK_FILE

    return NULL;
}
