#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Process.h>
#include <iostream>
#include <string>
#include <vector>

#include "globals.h"
#include "cache.h"

using namespace Poco::Net;
using namespace Poco::Util;
using namespace std;

cache_t cache;

class MyRequestHandler : public HTTPRequestHandler
{
    public:
        virtual void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
        {
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
                ostream& out = resp.send();
                out << "Undetected!";
                out.flush();
            }
        }

        void head(HTTPServerRequest& req, HTTPServerResponse &resp) {
            debug("got a head");

            // simply return a header
            resp.setStatus(HTTPResponse::HTTP_OK);
            resp.setContentType("text/html");
            ostream& out = resp.send();
            out << "head";
            out.flush();
        }

        void put(HTTPServerRequest& req, HTTPServerResponse &resp) {
            debug("got a put");

            // parse uri for format.
            // should be "/<key>/<value>"
            std::string uri = req.getURI();
            std::vector<std::string> tokens = string_split(uri, '/');
            if (tokens.size() != 3 || tokens[0] != "") {
                debug("bad put request");
                error(req, resp);
            } else {
                // valid request. put key van value into cache
                key_type key = (key_type) tokens[1].c_str();
                val_type val = (val_type) tokens[2].c_str();
                uint32_t val_size = strlen(tokens[2].c_str());

                cache_set(cache, key, val, val_size);

                // generate response
                resp.setStatus(HTTPResponse::HTTP_OK);
                resp.setContentType("text/html");
                ostream& out = resp.send();
                out << "put";
                out.flush();
            }
        }

        void handle_delete(HTTPServerRequest& req, HTTPServerResponse &resp) {
            // would call delete, but it is a keyword in c++
            std::cout << "delete" << std::endl;

            // parse input for "/<key>"
            std::string uri = req.getURI();
            std::vector<std::string> tokens = string_split(uri, '/');
            if (tokens.size() != 2 || tokens[0] != "") {
                // bad request form. send back error
                debug("bad get request");
                error(req, resp);
            }

            key_type key = (key_type) tokens[1].c_str();
            cache_delete(cache, key);

            resp.setStatus(HTTPResponse::HTTP_OK);
            resp.setContentType("text/html");
            ostream& out = resp.send();
            out << "delete";
            out.flush();
        }

        void get(HTTPServerRequest& req, HTTPServerResponse &resp) {
            std::cout << "get" << std::endl;

            // parse uri for format.
            // should be "/<key>/<value>"
            std::string uri = req.getURI();
            std::vector<std::string> tokens = string_split(uri, '/');
            if (tokens.size() != 2 || tokens[0] != "") {
                // bad request form. send back error
                debug("bad get request");
                error(req, resp);
            } else {
                // get val from cache
                key_type key = (key_type) tokens[1].c_str();
                uint32_t val_size;
                val_type val = cache_get(cache, key, &val_size);

                if (!val) {
                    // if key is not in cache
                    debug("key not in cache");
                    error(req, resp);
                } else {
                    // otherwise key is in cache
                    // convert val_type (i.e void*) into a std::string
                    char new_val[val_size + 1];
                    memcpy(new_val, val, val_size);
                    new_val[val_size] = '\0';
                    std::string str_val = std::string(new_val);

                    // return response with uri:{key: k, value: v } 
                    resp.setStatus(HTTPResponse::HTTP_OK);
                    resp.setContentType("text/html");
                    ostream& out = resp.send();
                    out << "{key: "
                        << key
                        << ", value: "
                        << str_val
                        << "}";
                    out.flush();
                }
            }
        }

        void post(HTTPServerRequest& req, HTTPServerResponse &resp) {
            std::cout << "post" << std::endl;

            // check for shutdown command
            std::string uri = req.getURI();
            if (uri == "/shutdown") {
                resp.setStatus(HTTPResponse::HTTP_OK);
                resp.setContentType("text/html");
                ostream& out = resp.send();
                out << "POST";
                out.flush();
                Poco::Process::requestTermination(Poco::Process::id());

            } else {
                error(req, resp);
            }
        }

        void error(HTTPServerRequest& req, HTTPServerResponse &resp) {
            resp.setStatus(HTTPResponse::HTTP_OK);
            resp.setContentType("text/html");
            ostream& out = resp.send();
            out << "error";
            out.flush();
        }
};

class MyRequestHandlerFactory : public HTTPRequestHandlerFactory
{
    public:
        virtual HTTPRequestHandler* createRequestHandler(const HTTPServerRequest &request)
        {
            return new MyRequestHandler; 
        }
};

class Server : public ServerApplication
{
    public:
        void start() {
            cache = create_cache(10000);
            run();
        }

    protected:
        int main(const vector<string> &)
        {
            HTTPServer s(new MyRequestHandlerFactory, ServerSocket(globals::PORT), new HTTPServerParams);
            s.start();
            waitForTerminationRequest();  // wait for CTRL-C or kill
            s.stopAll();
            return Application::EXIT_OK;
        }
};

