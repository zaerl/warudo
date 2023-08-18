#include "http.h"
#include "net.h"
#include "warudo.h"

warudo_code warudo_page_home(warudo *config) {
    CHECK_CONFIG

    return warudo_server_error("Warudo is a Fast CGI application. "
        "You need to use a webserver and proxy the requests to the socket. "
        "Follow the instructions found in README.md.", config);
}
