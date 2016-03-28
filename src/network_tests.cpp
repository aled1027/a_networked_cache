
#include <iostream>
#include "network_server.h"
#include "network_client.h"
#include "network_tests.h"


void client_test() {
    HTTPClient c;
    c.request("GET", "/3", "version");
}

void server_test() {
    HTTPServer s;
    s.listen();
}

void network_tests(std::string user) {
    std::cout << "Running network_tests" << std::endl;
    if (user.compare("client") == 0) {
        client_test();
    } else {
        server_test();
    }
}
