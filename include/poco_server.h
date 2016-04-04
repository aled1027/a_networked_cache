#pragma once 

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>

using namespace Poco::Net;
using namespace Poco::Util;

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

class Server : public ServerApplication
{
    public:
        void start();

    protected:
        int main(const std::vector<std::string> &);
};
