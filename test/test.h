#ifndef WRD_TEST_H
#define WRD_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/warudo.h"
#include "attractor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOCK_CONFIG warudo config;
#define MOCK_FILE(NAME) FILE *NAME = tmpfile(); if(!NAME) ATT_ASSERT(0, 1, "tmpfile() failed");

#define ASSERT_CODE(RESULT, EXPECTED, DESCRIPTION) wrd_assert_code(RESULT, EXPECTED, DESCRIPTION);

void wrd_assert_code(int result, int expected, const char *description);

#ifdef __cplusplus
}
#endif

#endif /* WRD_TEST_H */
