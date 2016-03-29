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
    cache_t cache = NULL;
    public:
    virtual void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
    {
        if (req.getMethod().compare("POST") == 0) {
            post(req, resp);
        } else if (req.getMethod().compare("GET") == 0) {
            get(req, resp);
        } else {
            ostream& out = resp.send();
            out << "<h1>Undetected!</h1>";
            out.flush();
        }
    }

    void get(HTTPServerRequest& req, HTTPServerResponse &resp) {
        std::cout << "get" << std::endl;
        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.setContentType("text/html");
        ostream& out = resp.send();
        out << "<h1>GET</h1>";
        out.flush();
    }

    void post(HTTPServerRequest& req, HTTPServerResponse &resp) {
        // parse URI (the body). Should be "/shutdown"
        std::string uri = req.getURI();

        if (uri.compare("/shutdown") == 0) {
            resp.setStatus(HTTPResponse::HTTP_OK);
            resp.setContentType("text/html");
            ostream& out = resp.send();
            out << "<h1>POST</h1>";
            out.flush();
            //terminate();
            Poco::Process::requestTermination(Poco::Process::id());

        } else {
            error(req, resp);
        }
    }

    void error(HTTPServerRequest& req, HTTPServerResponse &resp) {
        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.setContentType("text/html");
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
            cache = create_cache(128);
            run();
        }

    protected:
        int main(const vector<string> &)
        {
            std::cout << "in protect main" << std::endl;
            HTTPServer s(new MyRequestHandlerFactory, ServerSocket(globals::PORT), new HTTPServerParams);
            s.start();
            waitForTerminationRequest();  // wait for CTRL-C or kill
            s.stopAll();
            return Application::EXIT_OK;
        }

};

