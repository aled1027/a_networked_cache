#include <iostream>
#include <string>
#include <vector>

#include <Poco/Net/ServerSocket.h>
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/MulticastSocket.h"
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "Poco/DateTimeFormatter.h"
#include <Poco/Mutex.h>
#include <Poco/Process.h>
#include "Poco/Runnable.h"
#include <Poco/Task.h>
#include <Poco/TaskManager.h>
#include "Poco/Timestamp.h"
#include <Poco/Timespan.h>
#include <Poco/URI.h>
#include <Poco/Util/ServerApplication.h>

#include <assert.h>
#include "globals.h"
#include "cache.h"

#include "poco_server.h"

using namespace Poco::Net;
using namespace Poco::Util;
using Poco::Mutex;

cache_t cache;

class MyRequestHandler : public HTTPRequestHandler
{
    public:
        virtual void handleRequest(HTTPServerRequest& req, HTTPServerResponse& resp);
    private:
        void head(HTTPServerRequest& req, HTTPServerResponse &resp);
        void put(HTTPServerRequest& req, HTTPServerResponse &resp);
        void handle_delete(HTTPServerRequest& req, HTTPServerResponse &resp);
        void get(HTTPServerRequest& req, HTTPServerResponse &resp);
        void post(HTTPServerRequest& req, HTTPServerResponse &resp);
        void ok(HTTPServerRequest& req, HTTPServerResponse &resp, std::string& body);
        void not_found(HTTPServerRequest& req, HTTPServerResponse &resp);
        void bad_request(HTTPServerRequest& req, HTTPServerResponse &resp);
};

class MyRequestHandlerFactory : public HTTPRequestHandlerFactory
{
    public:
        virtual HTTPRequestHandler* createRequestHandler(const HTTPServerRequest &request);
};

class MyTCPServer : public ServerApplication
{
    public:
        void start();

    protected:
        int main(const std::vector<std::string> &);
};

class MyUDPServer : public Poco::Task {
    public:
        MyUDPServer(const std::string& name) : Task(name) {}
        void runTask() {
            debug("MyUDPServer::runTask");

            //create socket address, bind to dgs, set timeouts
            Poco::Net::SocketAddress sa(globals::HOST, globals::UDP_PORT);
            Poco::Net::DatagramSocket dgs(sa, true);
            dgs.setSendTimeout(Poco::Timespan(0,0,0,0,5000));
            dgs.setReceiveTimeout(Poco::Timespan(0,0,0,0,5000));

            char buffer[2048];
            while (true) {
                // if thread is cancelled from outside, return.
                if (isCancelled()) {
                    return;
                }

                // try to receive a message.
                // if it fails with a poco exception - aka a time out
                // go to beginning of loop
                Poco::Net::SocketAddress sender;
                int n = 0;
                try {
                    n = dgs.receiveFrom(buffer, sizeof(buffer)-1, sender);
                    buffer[n] = '\0';

                    std::ostringstream debug_recv;
                    debug_recv << "server::cache_get (udp) got: " << sender.toString() << ": " << buffer;
                    debug(debug_recv.str());
                    // std::cout << "<" << buffer << ">" << std::endl;
                } catch (Poco::Exception &e) {
                    continue;
                }

                // get key from cache
                key_type key;
                val_type val;
                uint32_t val_size;

                //if we are using CPP Client, get key from path
                if (!globals::IS_PYTHON_CLIENT) {
                    std::string decoded_uri;
                    Poco::URI::decode(buffer, decoded_uri);
                    std::vector<std::string> tokens = string_split(decoded_uri, '/');
                    if (tokens.size() != 2 || tokens[0] != "") {
                        debug("bad request");
                    }
                    key = (key_type) strdup(tokens[1].c_str());
                } else { 
                    //if we are using python UDP, get key from message
                    debug("ok python client");
                    std::string str_buffer(buffer);
                    key = (key_type) strdup(str_buffer.c_str());
                }

                // Resume normal track
                val = cache_get(cache, key, &val_size);
                std::ostringstream db;
                db << "{\"key\": \"" << key << "\", \"value\": \"" << val << "\"}";
                debug(db.str());

                if (!val) {
                    // if key is not in cache
                    debug("key not in cache");
                    // do nothing...? or return "NULL" (empty) value
                    std::ostringstream oss;
                    oss << "{\"key\": \"" << key << "\", \"value\": \"" << "NULL" << "\"}";
                    std::string msg = oss.str();
                    dgs.sendTo(msg.data(), msg.size(), sender);

                } else {
                    // otherwise key is in cache
                    // convert val_type (i.e void*) into a char*
                    debug("successful get");

                    char new_val[val_size + 1];
                    memcpy(new_val, val, val_size);
                    new_val[val_size] = '\0';

                    // return response with uri:{key: k, value: v } 
                    std::ostringstream oss2;
                    oss2 << "{\"key\": \"" << key << "\", \"value\": \"" << new_val << "\"}";
                    std::string msg = oss2.str();
                    std::ostringstream debug_get;
                    debug_get << "returning: " << msg;
                    debug(debug_get.str());
                    dgs.sendTo(msg.data(), msg.size(), sender);
                }
                debug("got to the end!");
                free((char*) key);
                free((void*) val);
            }
        }
};


void MyRequestHandler::handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
{
    debug("got a request");

    // lock
    //{
    //    Mutex::ScopedLock lock(globals::mutex);
    //    {
            if (req.getMethod() == "POST") {
                post(req, resp);
            } else if (req.getMethod() == "GET") {
                get(req, resp);
            } else if (req.getMethod() == "PUT") {
                put(req, resp);
            } else if (req.getMethod() == "DELETE") {
                handle_delete(req, resp); // delete is a keyword in c++
            } else if (req.getMethod() == "HEAD") {
                head(req, resp); 
            } else {
                bad_request(req, resp);
            }
    //    }
    //}
}


void MyRequestHandler::head(HTTPServerRequest& req, HTTPServerResponse &resp) {
    debug("got a head");
    std::string body("head");
    ok(req, resp, body);
}

void MyRequestHandler::put(HTTPServerRequest& req, HTTPServerResponse &resp) {
    debug("got a put");

    // parse uri for format.
    // should be "/<key>/<value>"
    std::string uri = req.getURI();
    std::string decoded_uri;
    Poco::URI::decode(uri, decoded_uri);

    std::vector<std::string> tokens = string_split(decoded_uri, '/');
    if (tokens.size() != 3 || tokens[0] != "") {
        debug("bad put request");
        bad_request(req, resp);
    } else {
        // valid request. put key and value into cache
        key_type key = (key_type) tokens[1].c_str();
        val_type val = (val_type) tokens[2].c_str();
        uint32_t val_size = strlen(tokens[2].c_str());

        cache_set(cache, key, val, val_size);

        //send ok response
        std::string body("put");
        ok(req, resp, body);
        debug("set in cache");
    }
}

void MyRequestHandler::handle_delete(HTTPServerRequest& req, HTTPServerResponse &resp) {
    // would call delete, but it is a keyword in c++
    debug("got a delete");

    // parse input for "/<key>"
    std::string uri = req.getURI();
    std::string decoded_uri;
    Poco::URI::decode(uri, decoded_uri);

    std::vector<std::string> tokens = string_split(decoded_uri, '/');
    if (tokens.size() != 2 || tokens[0] != "") {
        // bad request form. send back bad_request
        debug("bad get request");
        bad_request(req, resp);
    }

    key_type key = (key_type) tokens[1].c_str();
    cache_delete(cache, key);
    std::string body("delete");
    ok(req, resp, body);
    debug("deleted");
}

void MyRequestHandler::get(HTTPServerRequest& req, HTTPServerResponse &resp) {
    debug("got a get");

    // parse uri for format.
    // should be "/<key>/<value>"
    std::string uri = req.getURI();
    std::string decoded_uri;
    Poco::URI::decode(uri, decoded_uri);

    std::vector<std::string> tokens = string_split(decoded_uri, '/');
    if (tokens.size() != 2 || tokens[0] != "") {
        // bad request form. send back bad_request
        debug("bad get request");
        bad_request(req, resp);
    } else if (tokens[1] == "memused") {
        debug("get request for memsize");

        uint64_t memused = cache_space_used(cache);
        std::string memused_str = std::to_string(memused);
        //std::cout << "memused_str: " << memused_str << std::endl;

        std::ostringstream oss;
        oss << "{\"memused\": \"" << memused_str << "\"}";
        std::string body = oss.str();
        //std::cout << "head body is: " << body << std::endl;
        ok(req, resp, body);
    } else {
        // get val from cache
        key_type key = (key_type) tokens[1].c_str();
        uint32_t val_size;
        val_type val = cache_get(cache, key, &val_size);

        if (!val) {
            // if key is not in cache
            debug("key not in cache");
            not_found(req, resp);
        } else {
            // otherwise key is in cache
            // convert val_type (i.e void*) into a std::string
            debug("successful get");

            char new_val[val_size + 1];
            memcpy(new_val, val, val_size);
            new_val[val_size] = '\0';
            std::string str_val = std::string(new_val);

            // uri encoding currently assumes that spaces are the only reserved char
            std::string encoded_val;
            std::string encoded_key;
            Poco::URI::encode(tokens[1], "", encoded_key);
            Poco::URI::encode(str_val, "", encoded_val);

            // return response with uri:{key: k, value: v } 
            std::ostringstream oss;
            oss << "{\"key\": \"" << encoded_key << "\", \"value\": \"" << encoded_val << "\"}";
            std::string body = oss.str();
            ok(req, resp, body);
        }
    }
}

void MyRequestHandler::post(HTTPServerRequest& req, HTTPServerResponse &resp) {
    debug("got a post");

    // check for shutdown command
    std::string uri = req.getURI();
    if (uri == "/shutdown") {
        debug("got a shutdown");
        std::string body("shutdown");
        ok(req, resp, body);
        Poco::Process::requestTermination(Poco::Process::id());
        return;
    } 

    // check for new maxmem value: /memsize/value
    std::vector<std::string> tokens = string_split(uri, '/'); 

    // check format
    if (tokens.size() != 3 || tokens[0] != "" || tokens[1] != "memsize") {
        debug("post parsing error");
        bad_request(req, resp);
        return;
    }

    // check if cache is already being used
    if (cache_space_used(cache) != 0) {
        debug("post cache space used != 0");
        bad_request(req, resp);
        return;
    }

    // otherwise go ahead and change the maxmem as requested
    uint64_t new_maxmem = std::stoull(tokens[2]);
    destroy_cache(cache);
    cache = NULL;
    cache = create_cache(new_maxmem);
    assert(cache);

    std::string body("changed maxmem");
    ok(req, resp, body);
}

void MyRequestHandler::ok(HTTPServerRequest& req, HTTPServerResponse &resp, std::string& body) {
    resp.setStatus(HTTPResponse::HTTP_OK);
    resp.setContentType("text/html");
    std::ostream& out = resp.send();
    out << body;
    out.flush();
}

void MyRequestHandler::not_found(HTTPServerRequest& req, HTTPServerResponse &resp) {
    resp.setStatus(HTTPResponse::HTTP_NOT_FOUND);
    resp.setContentType("text/html");
    std::ostream& out = resp.send();
    out << "not found";
    out.flush();
}

void MyRequestHandler::bad_request(HTTPServerRequest& req, HTTPServerResponse &resp) {
    resp.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
    resp.setContentType("text/html");
    std::ostream& out = resp.send();
    out << "bad request";
    out.flush();
}

HTTPRequestHandler* MyRequestHandlerFactory::createRequestHandler(const HTTPServerRequest &request)
{
    return new MyRequestHandler; 
}

void MyTCPServer::start() {
    cache = create_cache(globals::DEFAULT_MAXMEM);
    run();
}

int MyTCPServer::main(const std::vector<std::string> &)
{
    HTTPServer s(new MyRequestHandlerFactory, ServerSocket(globals::TCP_PORT), new HTTPServerParams);
    s.start();
    waitForTerminationRequest();  // wait for CTRL-C or kill
    s.stopAll();
    return Application::EXIT_OK;
}

void Server::start() 
{
    if (globals::USE_UDP) {
        debug("initializing server with TCP and UDP");
        Poco::TaskManager task_manager;

        for (uint32_t i = 0; i < globals::NUM_UDP_THREADS; i++) {
            task_manager.start(new MyUDPServer("task"));
        }

        MyTCPServer tcp_server;
        tcp_server.start();

        task_manager.cancelAll();
        task_manager.joinAll();
    } else {
        debug("initializing server with TCP");
        MyTCPServer tcp_server;
        tcp_server.start();
    }
}

