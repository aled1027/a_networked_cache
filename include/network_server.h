#pragma once

#include "globals.h"
#include <cstring>
#include <string>
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
        cache_t cache;
        HTTPServer() {
            debug("HTTPServer::HTTPServer");
            cache = create_cache(128);
        }

        ~HTTPServer() {
            debug("HTTPServer::~HTTPServer");
            destroy_cache(cache);
        }

        void listen() {
            debug("HTTPServer::listen");

            std::string raw_request;
            while (true) {
                // loops until a shutdown request.
                
                // wait for request
                raw_request = "";
                do {
                    raw_request = network_server.receive();
                } while (raw_request.compare("") == 0);

                bool shutdown = process_request(raw_request);

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
                    send_ok();
                    return true; // true indicates shutdown
                }
            } else if (request.method.compare("PUT") == 0) {
                std::cout << "IN PUT" << std::endl;
                std::vector<std::string> tokens = string_split(request.path, '/');

                // TODO check for bad input
                
                std::cout << tokens[0] << std::endl; 
                std::cout << tokens[1] << std::endl;
                std::cout << tokens[2] << std::endl;

                key_type key = new char[tokens[1].size()+1];
                strcpy (key, tokens[1].c_str());
                key_type val = new char[tokens[2].size()+1];
                strcpy (val, tokens[2].c_str());
                uint32_t val_size = sizeof(tokens[2]);

                printf("%s\n", key);
                printf("%s\n", val);
                std::cout << val_size << std::endl;

                //uint32_t size = sizeof(tokens[1]);


                //cache_set(cache, &tokens[0], &tokens[1], sizeof(tokens[1]));
            } else if (request.method.compare("DELETE") == 0) {
            } else if (request.method.compare("HEAD") == 0) {
            } else {
                send_bad_request();
            }

            send_bad_request();
            return false;
        }


        void send_ok() {
            network_server.send("HTTP/1.1 200 OK");
        }

        void send_bad_request() {
            network_server.send("HTTP/1.1 400 Bad Request\n");
        }
        
};
