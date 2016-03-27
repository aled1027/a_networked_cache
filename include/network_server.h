#pragma once

#include "globals.h"

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
            debug("NetworkServer::receive");
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

            while (true) {
                // loops until a shutdown request.
                // wait for request
                std::string raw_request;
                do {
                    raw_request = network_server.receive();
                } while (raw_request.compare("") == 0);

                // process request
                process_request(parse_request(raw_request));
            }
        }

        void process_request(http_info hi) {
            debug("HTTPServer::process_request");

            if (hi.method.compare("GET")) {
            } else if (hi.method.compare("POST")) {
            } else if (hi.method.compare("PUT")) {
            } else if (hi.method.compare("DELETE")) {
            } else if (hi.method.compare("HEAD")) {
            } else {
                network_server.send("HTTP/1.0 400 Bad Request\n");
            }

            std::string response = "HTTP/1.0 400 Bad Request\n";
            network_server.send(response);
        }

        
};
