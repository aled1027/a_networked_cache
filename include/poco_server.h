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
#include "cache.h"

using namespace Poco::Net;
using namespace Poco::Util;
using namespace std;

class MyGetHandler : public HTTPRequestHandler {
public:
    virtual void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp) {
        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.setContentType("text/html");
        ostream& out = resp.send();
        out << "<h1>"
            << req.getURI()
            << "<h1>";
        out.flush();
    }
};

class MyRequestHandler : public HTTPRequestHandler
{
    cache_t cache = NULL;
    public:
    virtual void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
    {
        if (req.getMethod().compare("POST") == 0) {
            post(req, resp);
        } else {
            ostream& out = resp.send();
            out << "<h1>Undetected!</h1>";
            out.flush();
        }
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
            if (request.getMethod() == "GET") {
                std::cout << "in here" << std::endl;
                return new MyGetHandler;
            } else if (request.getMethod() == "POST") {
                return new MyRequestHandler; 
            } else {
                return new MyRequestHandler; 
            }
        }
};

class Server : public ServerApplication
{
    protected:
        int main(const vector<string> &)
        {
            std::cout << "in protect main" << std::endl;
            HTTPServer s(new MyRequestHandlerFactory, ServerSocket(9090), new HTTPServerParams);
            s.start();
            waitForTerminationRequest();  // wait for CTRL-C or kill
            s.stop();
            return Application::EXIT_OK;
        }

};


void poco_server_go(int argc, char *argv[]) {
    Server s;
    s.run(argc, argv);
}
