#include "../src/conf.h"
#include "test.h"

void *test_conf(void *arg) {
    MOCK_CONFIG
    MOCK_CONFIG_S

    ATT_ASSERT(wrd_load_config(&config, NULL), WRD_DEFAULT, "wrd_load_config NULL")

    WRD_MOCK_FILE("")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_DEFAULT, "wrd_load_config empty")

    WRD_MOCK_FILE("\n")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config empty JSON")

    WRD_MOCK_FILE("{test")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_DB_ERROR, "wrd_load_config malformed JSON")

    WRD_MOCK_FILE("{}")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config valid JSON")

    WRD_MOCK_FILE("{ \"test\": true }")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config valid JSON")

    WRD_MOCK_FILE("{ // Test\ntest: 1, }")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config valid JSON5")

    WRD_MOCK_FILE("{ access_origin: 'a test' }")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config add access_origin")
    ATT_ASSERT(config.access_origin, "a test", "wrd_load_config valid access_origin")

    WRD_MOCK_FILE("{ net_buffer_size: 100 }")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config add net_buffer_size")
    ATT_ASSERT(config.access_origin, NULL, "wrd_load_config access_origin NULL")
    ATT_ASSERT(config.net_buffer_size, 100, "wrd_load_config valid net_buffer_size")

    WRD_MOCK_FILE("{ net_buffer_size: '30' }")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config add net_buffer_size")
    ATT_ASSERT(config.net_buffer_size, WRD_DEFAULT_NET_BUFFER_SIZE, "wrd_load_config default net_buffer_size")

    // WRD_MOCK_FILE("test=test")
    // ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), 1, "wrd_load_config test 4")

    FREE_MOCK_FILE

    return NULL;
}
