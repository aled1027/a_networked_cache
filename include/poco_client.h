#pragma once

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPClientSession.h>

#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/Exception.h>
#include <Poco/URI.h>

// used to parse JSON
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>  // for copy
#include <iterator>

#include "cache.h"

// #ifndef GLOBALS_H
// #define GLOBALS_H
#include "globals.h"
// #endif

// using namespace Poco::Net;
// using namespace Poco::Util;

// struct cache_obj
// {
//     int port;
//     const char* host;
// };

// typedef const uint8_t *key_type;
// typedef const void *val_type;
// typedef struct cache_client_t *cache_client;
// typedef struct cache_obj *cache_t;

class Client {
    public:

        Client() {}
        ~Client() {}

        std::string send(cache_t cache, std::string& uri_str, const std::string& method, Poco::Net::HTTPResponse& res);

        cache_t create_cache(uint64_t maxmem);

        val_type cache_get(cache_t cache, const uint8_t* key);

        void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size);

        void cache_delete(cache_t cache, key_type key);

        uint64_t cache_space_used(cache_t cache);

        void destroy_cache(cache_t cache);
};
