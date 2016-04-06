#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/Exception.h>
#include <Poco/URI.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/MulticastSocket.h"
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/DatagramSocket.h>
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include <iostream>

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
    cache->port = globals::TCP_PORT;

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
            debug("got ok");
            break;
        default:
            debug("got not ok");
            break;
    }
    return cache;
}

val_type Client::cache_get(cache_t cache, const uint8_t* key) {
    if (globals::USE_UDP) {
        debug("client::cache_get with udp");
        Poco::Net::SocketAddress sa_send("localhost", globals::UDP_PORT1);
        Poco::Net::DatagramSocket dgs_send;
        dgs_send.connect(sa_send);

        Poco::Net::SocketAddress sa_recv("localhost", globals::UDP_PORT2);
        Poco::Net::DatagramSocket dgs_recv(sa_recv);
        dgs_recv.setReceiveTimeout(Poco::Timespan(0,0,0,1,0));

        Poco::Timestamp now;
        std::string msg = Poco::DateTimeFormatter::format(now,
                "<14>%w %f %H:%M:%S Hello, world!");
        //std::string msg = "/key";
        dgs_send.sendBytes(msg.data(), msg.size());
        std::cout << "client sent" << std::endl;

        Poco::Net::SocketAddress sender;
        char buffer[2048];

        int n = 0;
        try {
            n = dgs_recv.receiveFrom(buffer, sizeof(buffer)-1, sender);
        } catch (Poco::Exception &e) {
            return NULL;
        }

        buffer[n] = '\0';
        std::cout << sender.toString() << ": " << buffer << std::endl;
        std::cout << "client received" << std::endl;
        return  NULL;
    } else {
        debug("client::cache_get with tcp");
        std::ostringstream oss;
        oss << "http://" << cache->host << ":" << cache->port << "/" << key;
        std::string uri_str = oss.str();

        // res will contain header information 
        // body will contain the body of the response. (white spaces are removed)
        Poco::Net::HTTPResponse res; 
        std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_GET, res);

        switch (res.getStatus()) {
            case Poco::Net::HTTPResponse::HTTP_OK:
                debug("got ok");
                break;
            default:
                debug("got not ok");
                break;
        }

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var parsed_json = parser.parse(body);
        Poco::JSON::Object::Ptr parsed_obj = parsed_json.extract<Poco::JSON::Object::Ptr>();

        Poco::Dynamic::Var val_var = parsed_obj->get("value");
        std::string val = val_var.convert<std::string>();

        // TODO maybe a memory leak. I think that this class owns the memory
        char *val_c = (char *)val.c_str();            
        char *buf = (char *) calloc(strlen(val_c) + 1, 1);
        memcpy(buf,val_c,strlen(val_c));
        return buf;
    }
}

void Client::cache_set(cache_t cache, key_type key, val_type val, uint32_t val_size){
    std::ostringstream oss;
    oss << "http://" << cache->host << ":" << cache->port << "/" << key << "/" << (uint8_t *)val;
    std::string uri_str = oss.str();
    Poco::Net::HTTPResponse res; 
    std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_PUT, res);
    switch(res.getStatus()) {
        case Poco::Net::HTTPResponse::HTTP_OK:
            debug("got ok");
            break;
        default:
            debug("got not ok");
            break;
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
            debug("cache_delete:: got ok. key deleted");
            break;
        default:
            debug("cache_delete:: got not-ok");
            break;
    }
}

uint64_t Client::cache_space_used(cache_t cache){
    std::ostringstream oss;
    oss << "http://" << cache->host << ":" << cache->port << "/memused";
    std::string uri_str = oss.str();

    Poco::Net::HTTPResponse res; 
    std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_GET, res);
    //std::string body = send(cache, uri_str, Poco::Net::HTTPRequest::HTTP_HEAD, res);

    switch(res.getStatus()) {
        case Poco::Net::HTTPResponse::HTTP_OK:
            break;
        default:
            debug("poco_client::cache_space_used got not-ok");
            return 0;
    }

    // if HTTP_OK:
    uint64_t val = 0;
    try {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var parsed_json = parser.parse(body);
        Poco::JSON::Object::Ptr parsed_obj = parsed_json.extract<Poco::JSON::Object::Ptr>();
        Poco::Dynamic::Var val_var = parsed_obj->get("memused");
        val = val_var.convert<Poco::UInt64>();
    } catch (Poco::Exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
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
            debug("destroy_cache:: got ok, shutting down...");
            std::cout <<"body: " << body << std::endl;
            break;
        default:
            debug("destroy_cache:: got not-ok");
            break;
    }
}

