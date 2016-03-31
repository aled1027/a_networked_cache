#include "poco_client.h"

struct cache_obj
{
    int port;
    const char* host;
};

    std::string Client::send(cache_t cache, std::string& uri_str, const std::string& method, Poco::Net::HTTPResponse& res) {
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

    cache_t Client::create_cache(uint64_t maxmem) {
        //create a cache object
        cache_t cache = (cache_t)calloc(1, sizeof(cache_t));
        cache->host = globals::HOST;
        cache->port = globals::PORT;

        //form the request
        std::ostringstream oss;
        oss << "http://" << cache->host << ":" << cache->port << "/memsize/" << maxmem;
        std::string uri_str = oss.str();

        //send the request (response is in res)
        Poco::Net::HTTPResponse res;
        std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_POST, res);

        //parse body
        switch(res.getStatus()) {
            case Poco::Net::HTTPResponse::HTTP_OK:
                std::cout << "cache_create:: got ok" << std::endl;
                std::cout <<"body:: " << body << std::endl;
                break;
            default:
                std::cout << "cache_create:: could not update maxmem " << std::endl;
        }
        return cache;
    }

    val_type Client::cache_get(cache_t cache, const uint8_t* key) {
        std::ostringstream oss;
        oss << "http://" << cache->host << ":" << cache->port << "/" << key;
        std::string uri_str = oss.str();

        // res will contain header information 
        // body will contain the body of the response. (white spaces are removed)
        Poco::Net::HTTPResponse res; 
        std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_GET, res);

        switch (res.getStatus()) {
            case Poco::Net::HTTPResponse::HTTP_OK:
                std::cout << "cache_get:: got ok" << std::endl;
                std::cout <<"body:: " << body << std::endl;
                break;
            default:
                std::cout << "cache_get:: got not-ok" << std::endl;
                break;
        }

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var parsed_json = parser.parse(body);
        Poco::JSON::Object::Ptr parsed_obj = parsed_json.extract<Poco::JSON::Object::Ptr>();

        Poco::Dynamic::Var val_var = parsed_obj->get("value");
        std::string val = val_var.convert<std::string>();

        std::cout << "cache_get:: value recieved: " << val << std::endl;


        char *val_c = (char *)val.c_str();            
        char *buf = (char *)calloc(strlen(val_c) + 1,1);
        memcpy(buf,val_c,strlen(val_c));

        return buf;
    }

    void Client::cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size){
        std::ostringstream oss;
        oss << "http://" << cache->host << ":" << cache->port << "/" << key << "/" << (uint8_t *)val;
        std::string uri_str = oss.str();

        Poco::Net::HTTPResponse res; 
        std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_PUT, res);
        switch(res.getStatus()) {
            case Poco::Net::HTTPResponse::HTTP_OK:
                std::cout << "cache_set:: got ok: "<< key << " : " << (uint8_t *)val << std::endl;
                std::cout <<"cache_set:: body: " << body << std::endl;
                break;
            default:
                std::cout << "cache_set:: got not-ok" << std::endl;
        }

    }

    void Client::cache_delete(cache_t cache, key_type key) {
        std::ostringstream oss;
        oss << "http://" << cache->host << ":" << cache->port << "/" << key;
        std::string uri_str = oss.str();

        Poco::Net::HTTPResponse res; 
        std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_DELETE, res);
        switch(res.getStatus()) {
            case Poco::Net::HTTPResponse::HTTP_OK:
                std::cout << "cache_delete:: got ok. key deleted: "<< key << std::endl;
                std::cout <<"body: " << body << std::endl;
                break;
            default:
                std::cout << "cache_delete:: got not-ok" << std::endl;
        }

    }

    uint64_t Client::cache_space_used(cache_t cache){
        std::ostringstream oss;
        oss << "http://" << cache->host << ":" << cache->port << "/";
        std::string uri_str = oss.str();

        Poco::Net::HTTPResponse res; 
        std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_HEAD, res);

        switch(res.getStatus()) {
            case Poco::Net::HTTPResponse::HTTP_OK:
                std::cout << "cache_space_used:: got ok."<< std::endl;
                std::cout <<"body:: " << body << std::endl;
                break;
            default:
                std::cout << "cache_space_used:: got not-ok" << std::endl;
        }

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var parsed_json = parser.parse(body);
        Poco::JSON::Object::Ptr parsed_obj = parsed_json.extract<Poco::JSON::Object::Ptr>();

        Poco::Dynamic::Var val_var = parsed_obj->get("memused");
        uint64_t val = val_var.convert<Poco::UInt64>();

        std::cout << "cache_spacedused:: memory used: " << val << std::endl;

        return val;
    }

    void Client::destroy_cache(cache_t cache) {
        std::ostringstream oss;
        oss << "http://" << cache->host << ":" << cache->port << "/shutdown";
        std::string uri_str = oss.str();

        Poco::Net::HTTPResponse res;
        std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_POST, res);

        switch (res.getStatus()) {
            case Poco::Net::HTTPResponse::HTTP_OK:
                std::cout << "destroy_cache:: got ok, shutting down..." << std::endl;
                std::cout <<"body: " << body << std::endl;
                break;
            default:
                std::cout << "destroy_cache:: got not-ok" << std::endl;
                break;
        }
    }

