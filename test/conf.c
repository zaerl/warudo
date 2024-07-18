#include "../src/conf.h"
#include "test.h"

void *test_conf(void *arg) {
    MOCK_CONFIG
    MOCK_CONFIG_S

    ATT_ASSERT(wrd_load_config(&config, NULL), WRD_DEFAULT, "wrd_load_config NULL")

    MOCK_FILE_S("")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_DB_ERROR, "wrd_load_config empty")

    MOCK_FILE_S("\n\n\n\n")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_DB_ERROR, "wrd_load_config empty")

    MOCK_FILE_S("test")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_DB_ERROR, "wrd_load_config test")

    MOCK_FILE_S("#")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_DB_ERROR, "wrd_load_config test 2")

    MOCK_FILE_S("test\ntest")
    ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), WRD_DB_ERROR, "wrd_load_config test 3")

    // MOCK_FILE_S("test=test")
    // ATT_ASSERT(wrd_load_config(&config, tmp_mock_file), 1, "wrd_load_config test 4")

    FREE_MOCK_FILE_S

    return NULL;
}
