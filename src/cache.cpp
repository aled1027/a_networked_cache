#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "evict.h"
#include "dbLL.h"
#include "cache.h"

#include <iostream>

const bool debug = false;

// What is this best practice for constants? Put them at top of file or in function?
const float RESET_LOAD_FACTOR = 0.1;
const float MAX_LOAD_FACTOR = 0.5;
typedef struct _dbLL_t hash_bucket;

struct cache_obj
{
    // http://stackoverflow.com/questions/6316987/should-struct-definitions-go-in-h-or-c-file
    uint64_t num_buckets;
    uint64_t memused;
    uint64_t maxmem;
    uint64_t num_elements;
    hash_bucket **buckets; // so buckets[i] = pointer to double linked list
    hash_func hash; // should only be accessed via cache_hash
    evict_t evict;

    // buckets[i] = pointer to double linked list
    // each node in double linked list is a hash-bucket
};

void print_cache(cache_t cache)
{
    printf("PRINTING CACHE OBJ\n");
    for (uint32_t i = 0; i < cache->num_buckets; ++i) {
        if (ll_size(cache->buckets[i]) > 0){
            printf("hash=%" PRIu32 " has dbll: \n", i);
            rep_list(cache->buckets[i]);
        }
    }
}

uint64_t modified_jenkins(key_type key)
{
    // https://en.wikipedia.org/wiki/Jenkins_hash_function
    uint32_t hash = *key;
    hash += (hash << 10);
    hash ^= (hash >> 6);
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return (uint64_t) hash;
}


static void print_key(key_type key)
{
    uint32_t i = 0;
    while (key[i]) {
        printf("%" PRIu32 ", ", key[i]);
        ++i;
    }
    printf("\n");
}

static uint64_t cache_hash(cache_t cache, key_type key)
{
    return cache->hash(key) % cache->num_buckets;
}

static void cache_dynamic_resize(cache_t cache)
{
    // dynamically resizes size of hash table, via changing num_buckets
    // and copying key-value pairs IF the current load factor exceeds

    float load_factor = (float)cache->num_elements / (float)cache->num_buckets;
    if (load_factor > MAX_LOAD_FACTOR) {
        uint64_t new_num_buckets = (uint64_t) ((float) cache->num_elements / RESET_LOAD_FACTOR);

        // new memory for new cache & initialize lists
        hash_bucket **new_buckets = (hash_bucket **)calloc(new_num_buckets, sizeof(hash_bucket*));
        assert(new_buckets && "memory");
        for (uint32_t i = 0; i < new_num_buckets; i++){
            new_buckets[i] = new_list();
        }

        // loop over all possible hash values
        for (uint64_t i = 0; i < cache->num_buckets; i++) {
            hash_bucket *dbll = cache->buckets[i];
            key_type *keys = ll_get_keys(dbll);
            uint32_t num_keys = ll_size(dbll);
            for (uint32_t j = 0; j < num_keys; j++) {
                uint32_t val_size;
                val_type val = ll_search(dbll, keys[j], &val_size);

                // insert key, val, val_size into dbll in new memory
                uint64_t new_hash = cache->hash(keys[j]) % new_num_buckets;
                hash_bucket *e = new_buckets[new_hash];
                ll_insert(e, keys[j], val, val_size);
                free((void *)val);
            }
            free(keys);
            destroy_list(cache->buckets[i]);
        }
        cache->num_buckets = new_num_buckets;
        free(cache->buckets);
        cache->buckets = new_buckets;
    }
}

cache_t create_cache(uint64_t maxmem)
{
    cache_t c = (cache_obj *)calloc(1, sizeof(struct cache_obj));

    c->memused = 0;
    c->maxmem = maxmem;
    c->num_buckets = 100;

    c->buckets = (hash_bucket **)calloc(c->num_buckets, sizeof(hash_bucket*));
    assert(c->buckets);
    for (uint32_t i = 0; i < c->num_buckets; i++){
        c->buckets[i] = new_list();
    }

    c->hash = modified_jenkins;
    c->evict = evict_create(c->num_buckets);
    return c;
}

void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size)
{
    if (val_size == 0) {
        return;
    }

    uint64_t hash = cache_hash(cache, key);
    hash_bucket *e = cache->buckets[hash]; // bucket the key belongs to

    // check if the key exists in the cache already
    uint32_t search_val_size;
    val_type res = ll_search(e, key, &search_val_size);
    if (res) {
        printf("KEY ALREADY EXISTS IN CACHE!!!! %s\n", key);
        // notify evict that key has been touched.
        // move to rear of LRU
        evict_get(cache->evict, key); 
        ll_insert(e, key, val, val_size); // insert into double linked list
        return;
    }

    // case where the value side being inserted is greater than max_mem
    if (val_size > cache->maxmem){
        printf("Could not store that value. Sorry!\n");
        printf("cache->maxmem: %" PRIu64 "; val_size: %d", cache->maxmem, val_size);
        return;
    }

    // eviction, if necessary
    while (cache->memused + val_size > cache->maxmem) {
        printf("cache_set calling evict set for removal\n");
        key_type k = evict_select_for_removal(cache->evict);
        assert(k && "if k is null, then our evict is empty and we shouldn't be removing anything");
        cache_delete(cache, k);
    }

    // insert <key,value> normally into cache
    ll_insert(e, key, val, val_size); // insert into double linked list
    evict_set(cache->evict, key); // notify evict object that key was inserted
    cache->memused += val_size;
    ++cache->num_elements;

    cache_dynamic_resize(cache); // will resize cache if load factor is exceeded
}

val_type cache_get(cache_t cache, key_type key, uint32_t *val_size)
{
    uint64_t hash = cache_hash(cache, key);

    if (debug) {
        printf("key = <%s>\n", key);
        printf("getting key = %" PRIu8 "\n", *key);
        printf("hash = %" PRIu64 "\n\n", hash);
    }

    hash_bucket *e = cache->buckets[hash];
    void *res = (void *) ll_search(e, key, val_size);
    if(res != NULL){
        evict_get(cache->evict, key);
    }
    return res;
}

void cache_delete(cache_t cache, key_type key)
{
    uint64_t hash = cache_hash(cache, key);
    hash_bucket *e = cache->buckets[hash];
    // val_size will be 0 only if there is no item to remove

    uint32_t val_size = ll_remove_key(e, key);
    if (val_size > 0) {
        printf("succesfully deleted %s\n", key);

        cache->memused -= val_size;
        --cache->num_elements;
        evict_delete(cache->evict, key);

        assert(cache->num_elements != uint32_t(-1));
    } else {
        printf("failed to deleted %s\n", key);
    }
}

uint64_t cache_space_used(cache_t cache)
{
    return cache->memused;
}

void destroy_cache(cache_t cache)
{
    for (uint32_t i = 0; i < cache->num_buckets; i++) {
        destroy_list(cache->buckets[i]);
    }

    evict_destroy(cache->evict);
    free(cache->evict);
    free(cache->buckets);
    cache->evict = NULL;
    cache->buckets = NULL;
    free(cache);
    cache = NULL;
}


