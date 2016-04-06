#pragma once

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

#include "cache.h"
#include "globals.h"

class Client {
    public:
        Client() {}
        ~Client() {}
        std::string send(cache_t cache, std::string& uri_str, 
                const std::string& method, Poco::Net::HTTPResponse& res);
        cache_t create_cache(uint64_t maxmem);
        val_type cache_get(cache_t cache, const uint8_t* key);
        void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size);
        void cache_delete(cache_t cache, key_type key);
        uint64_t cache_space_used(cache_t cache);
        void destroy_cache(cache_t cache);
};
