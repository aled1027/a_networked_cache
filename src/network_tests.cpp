
#include <iostream>
#include "network_server.h"
#include "network_client.h"
#include "network_tests.h"


void test_shutdown(bool is_client) {
    std::cout << "Running shutdown test" << std::endl;
    if (is_client) {
        HTTPClient c;
        c.request("POST", "/shutdown", "HTTP/1.1");
    } else {
        HTTPServer s;
        s.listen();
    }
}


void client_tests() { 
    bool is_client = true;
    test_shutdown(is_client);
}

void server_tests() { 
    bool is_client = false;
    test_shutdown(is_client);
}


void network_tests(std::string user) {
    std::cout << "Running network_tests" << std::endl;
    if (user.compare("client") == 0) {
        client_tests();
    } else {
        server_tests();
    }
}
