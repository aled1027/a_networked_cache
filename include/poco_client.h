#pragma once
#ifndef poco_client_h
#define poco_client_h

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPClientSession.h>

#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/Exception.h>
#include <Poco/URI.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>  // for copy
#include <iterator>

#include "globals.h"

// using namespace Poco::Net;
// using namespace Poco::Util;

struct cache_client_t
{
    int port;
    const char* host;
};

typedef const uint8_t *key_type;
typedef const void *val_type;
typedef struct cache_client_t *cache_client;

// // For a given key string, return a pseudo-random integer:
// typedef uint64_t (*hash_func)(key_type key);

// // Create a new cache object with a given maximum memory capacity.
// cache_t create_cache(uint64_t maxmem);

// // Add a <key, value> pair to the cache.
// // If key already exists, it will overwrite the old value.
// // If maxmem capacity is exceeded, sufficient values will be removed
// // from the cache to accomodate the new value.
// void cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size);

// // Retrieve the value associated with key in the cache, or NULL if not found.
// // The size of the returned buffer will be assigned to *val_size.
// val_type cache_get(cache_t cache, key_type key, uint32_t *val_size);

// // Delete an object from the cache, if it's still there
// void cache_delete(cache_t cache, key_type key);

// // Compute the total amount of memory used up by all cache values (not keys)
// uint64_t cache_space_used(cache_t cache);

// // Destroy all resource connected to a cache object
// void destroy_cache(cache_t cache);

class Client {
    public:

        Client() {}
        ~Client() {}

        std::string send(cache_client cache, std::string& uri_str, const std::string& method, Poco::Net::HTTPResponse& res) {
            // processes request with uri_str and method. Return response in res
            // with body in body
            try {
                Poco::Net::HTTPClientSession client_session(cache->host, cache->port);
                Poco::URI uri(uri_str);
                std::string path(uri.getPathAndQuery());

                Poco::Net::HTTPRequest req(method, path, Poco::Net::HTTPMessage::HTTP_1_1);
                client_session.sendRequest(req);

                // get response
                std::istream& istream_body = client_session.receiveResponse(res);
                std::stringstream ss_body;
                std::copy(std::istream_iterator<char>(istream_body),
                        std::istream_iterator<char>(),
                        std::ostream_iterator<char>(ss_body));
                std::string body = ss_body.str();
                return body;
            } catch (Poco::Exception &e) {
                std::cerr << "Exception: " << e.what() << std::endl;
                return e.what();
            }
        }



        val_type cache_get(cache_client cache, const uint8_t* key) {
            std::ostringstream oss;
            oss << "http://" << cache->host << ":" << cache->port << "/" << key;
            std::string uri_str = oss.str();

            // res will contain header information 
            // body will contain the body of the response. (white spaces are removed)
            Poco::Net::HTTPResponse res; 
            std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_GET, res);

            switch (res.getStatus()) {
                case Poco::Net::HTTPResponse::HTTP_OK:
                    std::cout << "cache_get: got ok" << std::endl;
                    std::cout <<"body: " << body << std::endl;
                    break;
                default:
                    std::cout << "cache_get: got not-ok" << std::endl;
                    break;
            }

            char *buf = (char *)"hi";            
            // char *buf = calloc(strlen(val) + 1,1);
            // memcpy(buf,val,strlen(val));

            return buf;
        }

        void cache_set(cache_client cache, key_type key, val_type val, uint32_t val_size){
            std::ostringstream oss;
            oss << "http://" << cache->host << ":" << cache->port << "/" << key << "/" << val;
            std::string uri_str = oss.str();

            Poco::Net::HTTPResponse res; 
            std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_PUT, res);
            switch(res.getStatus()) {
                case Poco::Net::HTTPResponse::HTTP_OK:
                    std::cout << "cache_set: got ok: "<< key << " : " << val << std::endl;
                    std::cout <<"body: " << body << std::endl;
                    break;
                default:
                    std::cout << "cache_set: got not-ok" << std::endl;
            }

        }

        void cache_delete(cache_client cache, key_type key) {
            std::ostringstream oss;
            oss << "http://" << cache->host << ":" << cache->port << "/" << key;
            std::string uri_str = oss.str();

            Poco::Net::HTTPResponse res; 
            std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_DELETE, res);
            switch(res.getStatus()) {
                case Poco::Net::HTTPResponse::HTTP_OK:
                    std::cout << "cache_delete: got ok. key deleted: "<< key << std::endl;
                    std::cout <<"body: " << body << std::endl;
                    break;
                default:
                    std::cout << "cache_delete: got not-ok" << std::endl;
            }

        }

        cache_client create_cache(uint64_t maxmem) {
            //create a cache object
            cache_client cache = (cache_client)calloc(1, sizeof(cache_client));
            cache->host = globals::HOST;
            cache->port = globals::PORT;

            //form the request
            std::ostringstream oss;
            oss << "http://" << cache->host << ":" << cache->port << "/memsize/" << &maxmem;
            std::string uri_str = oss.str();

            //send the request (response is in res)
            Poco::Net::HTTPResponse res;
            std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_POST, res);

            //parse body
            switch(res.getStatus()) {
                case Poco::Net::HTTPResponse::HTTP_OK:
                    std::cout << "cache_create: got ok" << std::endl;
                    std::cout <<"body: " << body << std::endl;
                    break;
                default:
                    std::cout << "cache_create: could not update maxmem " << std::endl;
            }
            return cache;
        }

        void destroy_cache(cache_client cache) {
            std::ostringstream oss;
            oss << "http://" << cache->host << ":" << cache->port << "/shutdown/";
            std::string uri_str = oss.str();

            Poco::Net::HTTPResponse res;
            std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_POST, res);

            switch (res.getStatus()) {
                case Poco::Net::HTTPResponse::HTTP_OK:
                    std::cout << "destroy_cache: got ok, shutting down..." << std::endl;
                    std::cout <<"body: " << body << std::endl;
                    break;
                default:
                    std::cout << "destroy_cache: got not-ok" << std::endl;
                    break;
            }
        }
};

#endif
