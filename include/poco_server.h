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
#include <Poco/Task.h>
#include <Poco/TaskManager.h>
#include <Poco/Thread.h>
#include <Poco/Timespan.h>



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

class MyUDPServer : public Poco::Task {
    public:
        MyUDPServer(const std::string& name) : Task(name) {}
        void runTask() {
            debug("MyUDPServer::runTask");

            // setup recv udp socket
            Poco::Net::SocketAddress sa_recv(globals::HOST, globals::UDP_PORT1);
            Poco::Net::DatagramSocket dgs_recv(sa_recv);
            dgs_recv.setReceiveTimeout(Poco::Timespan(0,0,0,0,5000));

            // setup sending udp socket
            Poco::Net::SocketAddress sa_send(globals::HOST, globals::UDP_PORT2);
            Poco::Net::DatagramSocket dgs_send;
            dgs_send.connect(sa_send);
            dgs_send.setSendTimeout(Poco::Timespan(0,0,0,0,5000));
            dgs_send.setReceiveTimeout(Poco::Timespan(0,0,0,0,5000));

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
                    n = dgs_recv.receiveFrom(buffer, sizeof(buffer)-1, sender);
                } catch (Poco::Exception &e) {
                    continue;
                }

                // if a message is successfully received, respond to it
                buffer[n] = '\0';
                std::cout << sender.toString() << ": " << buffer << std::endl;
                if (n > 0) {
                    Poco::Timestamp now;
                    std::string msg = Poco::DateTimeFormatter::format(now,
                            "<14>%w %f %H:%M:%S Hello, world!");
                    dgs_send.sendBytes(msg.data(), msg.size());
                    std::cout << "client sent" << std::endl;
                }
            }
            std::cout << "at end!" << std::endl;
        }
};

class Server {
    public:
        void start();
    private:
        MyTCPServer tcp_server;
        void start_udp();
};
