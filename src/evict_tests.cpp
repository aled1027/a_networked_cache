#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>

#include <iostream>
#include "dbLL_tests.h"
#include "cache_tests.h"
#include "evict.h"

#include "evict_tests.h"

#define my_assert(value, string) \
{if (!(value)) { printf("!!!FAILURE!!! %s\n", string);}}

void test_evict_object()
{
    printf("Running evict general test\n");
    evict_t evict = evict_create(10);

    uint8_t a[2] = {'a', '\0'};
    uint8_t b[2] = {'b', '\0'};
    uint8_t c[2] = {'c', '\0'};

    evict_set(evict, a);
    evict_get(evict, a);
    evict_set(evict, b);
    evict_set(evict, c);
    evict_delete(evict, a);
    evict_set(evict, c);

    key_type k = evict_select_for_removal(evict);
    my_assert(k && strcmp((const char*) k, (const char*) b) == 0, "didn't retrieve correct key");
    free((uint8_t*) k);

    evict_delete(evict, b);

    k = evict_select_for_removal(evict);
    my_assert(k && strcmp((const char*) k, (const char*) c) == 0, "didn't retrieve correct key");
    free((uint8_t*) k);

    evict_destroy(evict);
    free(evict);
}

void test_evict_duplicate_set() 
{
    printf("Running evict duplicate test\n");
    evict_t evict = evict_create(10);

    uint8_t a[2] = {'a', '\0'};

    evict_set(evict, a);
    evict_set(evict, a);

    key_type k = evict_select_for_removal(evict);
    my_assert(k && strcmp((const char*) k, (const char*) a) == 0, "didn't retrieve correct key");
    free((uint8_t*) k);

    evict_delete(evict, a);

    k = evict_select_for_removal(evict);
    my_assert(!k, "");
    free((uint8_t*) k);

    evict_destroy(evict);
    free(evict);
}

void evict_tests() 
{
    printf("***Running evict tests***\n");
    test_evict_object();
    test_evict_duplicate_set();
}

void new_evict_tests() {
    printf("***Running new evict tests***\n");
    EvictObject e;

    uint8_t a[2] = {'a', '\0'};
    uint8_t b[2] = {'b', '\0'};
    uint8_t c[2] = {'c', '\0'};


    e.set(a);
    e.set(a);
    e.set(a);
    e.set(a);
    e.get(a);
    e.set(b);
    e.remove(a);

    e.set(b);
    e.set(c);
    e.remove(b);
    e.set(a);
    e.remove(a);
    e.get(b);
    e.set(b);
    
    key_type k = e.select_for_removal();
    if (k) {
        my_assert(strcmp((char*) k, (char*) a), "a was removed, should not have been selected");
        std::cout << "removing " << k << std::endl;
        e.remove(k);
    }

    k = e.select_for_removal();
    if (k) {
        my_assert(strcmp((char*) k, (char*) a), "a was removed, should not have been selected");
        std::cout << "removing " << k << std::endl;
        e.remove(k);
    }

    k = e.select_for_removal();

    if (k) {
        my_assert(strcmp((char*) k, (char*) a), "a was removed, should not have been selected");
        std::cout << "removing " << k << std::endl;
        e.remove(k);
    }
    
    std::cout << "Probably passsed evict tests" << std::endl;
}



