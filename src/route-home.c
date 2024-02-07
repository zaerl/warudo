#include "http.h"
#include "output.h"
#include "warudo.h"

wrd_code wrd_page_home(warudo *config) {
    CHECK_CONFIG

    return wrd_server_error("Warudo is a Fast CGI application. "
        "You need to use a webserver and proxy the requests to the socket. "
        "Follow the instructions found in README.md.", config);
}
