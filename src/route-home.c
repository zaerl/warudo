#include "http.h"
#include "output.h"
#include "warudo.h"

wrd_code wrd_page_home(warudo *config) {
    CHECK_CONFIG

    return wrd_http_server_error(config, "Silence is golden");
}
