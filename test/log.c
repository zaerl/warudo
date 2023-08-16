#include "../src/log.h"
#include "../src/warudo.h"
#include "test.h"

void* test_log(void* arg) {
    MOCK_CONFIG
    printf("Testing log %d\n", config.log_level);

    ASSERT_CODE("NULL", WARUDO_ERROR, warudo_log, NULL, 0, "")
    ASSERT_CODE("NULL format", WARUDO_ERROR, warudo_log, &config, 0, NULL)
    ASSERT_CODE("no log", WARUDO_OK, warudo_log, &config, 0, "")

    config.log_level = WARUDO_LOG_LEVEL_ERROR;
    ASSERT_CODE("no log", WARUDO_OK, warudo_log, &config, WARUDO_LOG_LEVEL_ERROR, "")
    ASSERT_CODE("no log", WARUDO_OK, warudo_log, &config, WARUDO_LOG_LEVEL_INFO, "")
    ASSERT_CODE("no log", WARUDO_OK, warudo_log, &config, WARUDO_LOG_LEVEL_DEBUG, "")

    return NULL;
}
