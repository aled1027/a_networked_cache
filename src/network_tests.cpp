#include <iostream>

#include "poco_server.h"
#include "poco_client.h"
#include "network_tests.h"


void test_shutdown(bool is_client) {
    std::cout << "Running shutdown test" << std::endl;
    if (is_client) {
        Client c;
        cache_client cache = c.create_cache(15000);
        uint8_t key[6] = "hello";
        uint8_t val[6] = "world";
        c.cache_set(cache, key, val, 6);
        c.destroy_cache(cache);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_get(bool is_client) {
    std::cout << "Running get test" << std::endl;
    if (is_client) {
        Client c;
        cache_client cache = c.create_cache(15000);
        uint8_t key[6] = "hello";
        uint8_t val[6] = "world";
        c.cache_set(cache, key, val, 6);
        uint8_t *res = (uint8_t *)c.cache_get(cache, key);
        assert(strcmp((const char *)val, (const char *)res) && "get test failed");
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_put(bool is_client) {
    std::cout << "Running put test" << std::endl;
    if (is_client) {
        Client c;
        cache_client cache = c.create_cache(15000);
        uint8_t key[6] = "hello";
        uint8_t val[6] = "world";
        c.cache_set(cache, key, val, 6);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_update(bool is_client) {
    std::cout << "Running put test" << std::endl;
    if (is_client) {
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_delete(bool is_client) {
    std::cout << "Running delete test" << std::endl;
    if (is_client) {
        Client c;
        cache_client cache = c.create_cache(15000);
        uint8_t key[25] = "this key will be deleted";
        uint8_t val[11] = "delete me!";
        c.cache_set(cache, key, val, 11);
        c.cache_delete(cache, key);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_head(bool is_client) {
    std::cout << "Running head test" << std::endl;
    if (is_client) {
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_memsize(bool is_client) {
    std::cout << "Running head test" << std::endl;
    if (is_client) {
        Client c;
        cache_client cache = c.create_cache(15000);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void run_tests(bool is_client) { 
    // test_shutdown(is_client);
    //test_update(is_client);
    //test_head(is_client);
    test_memsize(is_client);
    test_put(is_client);
    test_get(is_client);
    test_delete(is_client);
    test_shutdown(is_client);
}

void network_tests(std::string user) {

    std::cout << "Running network_tests" << std::endl;
    if (user.compare("client") == 0) {
        run_tests(true);
    } else {
        run_tests(false);
    }
}
