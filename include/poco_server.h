#pragma once 

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>
#include "Poco/Runnable.h"
#include "Poco/Net/MulticastSocket.h"
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/DatagramSocket.h>
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"

#include "globals.h"

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

class MyTCPServer : public ServerApplication
{
    public:
        void start();
        void start_udp();

    protected:
        int main(const std::vector<std::string> &);
};

class MyUDPServer : public Poco::Runnable {
    bool keep_going;
    public:
    virtual void run() {
        debug("running udp");
        //keep_going = true;
        //Poco::Net::SocketAddress sa("localhost", 8081);
        //Poco::Net::DatagramSocket dgs(sa);

        //char buffer[2048];
        //while (keep_going) {
        //    Poco::Net::SocketAddress sender;
        //    //int n = dgs.receiveFrom(buffer, sizeof(buffer)-1, sender);
        //    //buffer[n] = '\0';
        //    //std::cout << sender.toString() << ": " << buffer << std::endl;

        //}
        
        Poco::Net::SocketAddress sa2("localhost", 8081);
        Poco::Net::DatagramSocket dgs2;
        dgs2.connect(sa2);
        Poco::Timestamp now;
        std::string msg = Poco::DateTimeFormatter::format(now,
                "<14>%w %f %H:%M:%S Hello, world!");
        dgs2.sendBytes(msg.data(), msg.size());

        std::cout << "sent" << std::endl;
    }



    void stop() {
        keep_going = false;
    }
};

class Server {
    public:
        void start();
    private:
        MyTCPServer tcp_server;
        void start_udp();
};
