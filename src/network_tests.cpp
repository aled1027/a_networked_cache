
#include <iostream>

#include "poco_client.h"
#include "poco_server.h"
#include "network_tests.h"


void test_shutdown(bool is_client) {
    std::cout << "Running shutdown test" << std::endl;
    if (is_client) {
    } else {
        Server s;
        s.start();
    }
}

void test_put(bool is_client) {
    std::cout << "Running put test" << std::endl;
    if (is_client) {
    } else {
        Server s;
        s.start();
    }
}

void test_delete(bool is_client) {
    std::cout << "Running delete test" << std::endl;
    if (is_client) {
    } else {
        Server s;
        s.start();
    }
}

void test_head(bool is_client) {
    std::cout << "Running head test" << std::endl;
    if (is_client) {
    } else {
        Server s;
        s.start();
    }
}

void run_tests(bool is_client) { 
    test_shutdown(is_client);
    test_put(is_client);
    test_delete(is_client);
    test_head(is_client);
}

void network_tests(std::string user) {
    std::cout << "Running network_tests" << std::endl;
    if (user.compare("client") == 0) {
        run_tests(true);
    } else {
        run_tests(false);
    }
}
