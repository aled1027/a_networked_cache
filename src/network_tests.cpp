
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

void test_put(bool is_client) {
    std::cout << "Running put test" << std::endl;
    if (is_client) {
        HTTPClient c;
        c.request("PUT", "/3/4", "HTTP/1.1");
        c.request("POST", "/shutdown", "HTTP/1.1");
    } else {
        HTTPServer s;
        s.listen();
    }
}


void run_tests(bool is_client) { 
    test_shutdown(is_client);
    test_put(is_client);
}


void network_tests(std::string user) {
    std::cout << "Running network_tests" << std::endl;
    if (user.compare("client") == 0) {
        run_tests(true);
    } else {
        run_tests(false);
    }
}
