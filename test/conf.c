#include "../src/conf.h"
#include "test.h"

void *test_conf(void *arg) {
    const char *config_data =
        "# This is a comment\n"
        "key1 = value1\n"
        "# Another comment\n"
        "key2=value2\n"
        "key3 =value3\n"
        "key4= value4\n";

    wrd_load_conf(config_data);

    return NULL;
}
