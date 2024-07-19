#include "../src/conf.h"
#include "test.h"

void *test_conf(void *arg) {
    MOCK_CONFIG
    MOCK_CONFIG_S

    ATT_ASSERT(wrd_load_config(&config, NULL), WRD_DEFAULT, "wrd_load_config NULL")

    MOCK_FILE_S("")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_DEFAULT, "wrd_load_config empty")

    MOCK_FILE_S("\n")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config empty JSON")

    MOCK_FILE_S("{test")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_DB_ERROR, "wrd_load_config malformed JSON")

    MOCK_FILE_S("{}")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config valid JSON")

    MOCK_FILE_S("{ \"test\": true }")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config valid JSON")

    MOCK_FILE_S("{ // Test\ntest: 1, }")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config valid JSON5")

    MOCK_FILE_S("{ access_origin: 'a test' }")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_OK, "wrd_load_config add access_origin")
    ATT_ASSERT(config.access_origin, "a test", "wrd_load_config valid access_origin")

    // MOCK_FILE_S("test=test")
    // ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), 1, "wrd_load_config test 4")

    FREE_MOCK_FILE_S

    return NULL;
}
