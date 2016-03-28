#pragma once

#include "globals.h"
#include "cache.h"
#include <boost/asio.hpp>

class NetworkServer {
    public:
        boost::asio::io_service io;
        boost::asio::ip::tcp::socket socket;
        boost::asio::ip::tcp::acceptor acceptor;
        NetworkServer() :
            socket(io),
            acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), globals::HELLO_PORT)) {
                debug("NetworkServer::NetworkServer");
                acceptor.accept(socket);
            }

        ~NetworkServer() {
            debug("NetworkServer::~NetworkServer");
            socket.close();
            acceptor.close();
        }

        std::string receive() {
            // receives a message. end of message is signified by "\n"
            //debug("NetworkServer::receive");
            boost::asio::streambuf sb;
            boost::system::error_code ec;
            boost::asio::read_until(socket, sb, "\n", ec);
            std::ostringstream ss;
            ss << &sb;
            return ss.str();
        }

        void send(const std::string& message) {
            // sends the message using tcp and socket as setup from before
            debug("NetworkServer::send");
            try {
                boost::system::error_code ignored_error;
                boost::asio::write(socket, boost::asio::buffer(message), boost::asio::transfer_all(), ignored_error);
            } catch(std::exception& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
            }
        }
};

class HTTPServer {
    public:
        NetworkServer network_server;
        HTTPServer() {
            debug("HTTPServer::HTTPServer");
        }

        ~HTTPServer() {
            debug("HTTPServer::~HTTPServer");
        }

        void listen() {
            debug("HTTPServer::listen");

            std::string raw_request("");
            while (true) {
                // loops until a shutdown request.
                // wait for request
                do {
                    raw_request = network_server.receive();
                } while (raw_request.compare("") == 0);

                // process request
                bool shutdown = process_request(raw_request);
                raw_request = "";

                if (shutdown)
                    break;
            }
        }

        bool process_request(std::string raw_request) {
            // return true if should shutdown else returns false
            http_info request = parse_request(raw_request);
            
            debug("HTTPServer::process_request");

            if (request.method.compare("GET") == 0) {
            } else if (request.method.compare("POST") == 0) {
                if (request.path.compare("/shutdown") == 0) {
                    network_server.send("HTTP/1.1 200 OK");
                    return true;
                }

            } else if (request.method.compare("PUT") == 0) {
            } else if (request.method.compare("DELETE") == 0) {
            } else if (request.method.compare("HEAD") == 0) {
            } else {
                network_server.send("HTTP/1.1 400 Bad Request\n");
            }

            std::string response = "HTTP/1.1 400 Bad Request\n";
            network_server.send(response);
            return false;
        }


        
};
