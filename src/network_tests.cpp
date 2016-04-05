#include "network_tests.h"

#include <unistd.h> // usleep

void my_assert(bool b, std::string str) {
    if (!b) {
        std::cout << str << std::endl;
    }
}

void call_sleep() {
    // gives time to server so they can setup
    // otherwise, the client tries to connect too quickly, and connection is refusedkj:w
    usleep(500000);
}

void test_shutdown(bool is_client) {
    std::cout << "Running shutdown test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        uint8_t key[8] = "goodbye";
        uint8_t val[8] = "friends";
        c.cache_set(cache, key, val, 8);
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
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);

        uint8_t key[6] = "hello";
        uint8_t val[6] = "world";
        c.cache_set(cache, key, val, 6);

        uint8_t *res = (uint8_t *) c.cache_get(cache, key);
        assert(!(strcmp((const char *)val, (const char *)res)) && "get test failed");

        c.destroy_cache(cache);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_put(bool is_client) {
    std::cout << "Running put test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
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

void test_update(bool is_client) {
    std::cout << "Running update test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        uint8_t key[6] = "hello";
        uint8_t val[6] = "world";
        c.cache_set(cache, key, val, strlen((char*) val));

        uint8_t val2[10] = "new_world";
        c.cache_set(cache, key, val2, strlen((char*) val2));

        uint8_t *res = (uint8_t *) c.cache_get(cache, key);
        my_assert(0 == strcmp((const char *)val2, (const char *)res), "get test failed");

        c.destroy_cache(cache);

    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_delete(bool is_client) {
    std::cout << "Running delete test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        uint8_t key[25] = "this key will be deleted";
        uint8_t val[11] = "delete me!";
        c.cache_set(cache, key, val, 11);
        c.cache_delete(cache, key);
        c.destroy_cache(cache);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_head(bool is_client) {
    std::cout << "Running head test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        uint32_t actual_mem_used = 0;

        uint8_t key_1[6] = "hello";
        uint8_t val_1[6] = "world";
        c.cache_set(cache, key_1, val_1, 6);
        actual_mem_used += strlen((char*) val_1);

        uint8_t key_2[8] = "another";
        uint8_t val_2[16] = "keyvaluepair";
        c.cache_set(cache, key_2, val_2, 16);
        actual_mem_used += strlen((char*) val_2);

        //we inserted two items, combined value size is 22, no evictions expected
        uint64_t res_mem_used = c.cache_space_used(cache);

        my_assert(actual_mem_used == res_mem_used, "memory used is not correct");
        c.destroy_cache(cache);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_memsize(bool is_client) {
    std::cout << "Running memsize test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        c.destroy_cache(cache);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void run_tests(bool is_client) { 
    //test_memsize(is_client); 
    //test_put(is_client); 
    test_get(is_client);
    //test_delete(is_client);
    //test_shutdown(is_client);
    //test_head(is_client); 
    //test_update(is_client); 
}

void network_tests(std::string user) {
    std::cout << "****************************" << std::endl;
    std::cout << "~  Running network_tests  ~" << std::endl;
    std::cout << "****************************\n" << std::endl;

    if (user == "client") {
        run_tests(true);
    } else {
        run_tests(false);
    }
}
