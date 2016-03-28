#pragma once

#include <cstring>
#include <string>
#include <boost/asio.hpp>

#include "cache.h"
#include "globals.h"
#include "http.h"

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
            //http_info request = parse_request(raw_request);
            Request request;
            request.from_str(raw_request);

            debug("HTTPServer::process_request");
            key_type key;
            val_type val;
            uint32_t val_size;
            std::vector<std::string> tokens;

            switch(request.method) {
                case GET:

                    // request should look like: "GET /key version"
                    tokens = string_split(request.path, '/');

                    // check if request.path is of form "/number"
                    if (tokens.size() != 2 || tokens[0].compare("") != 0) {
                        send_bad_request();
                        return false;
                    } 

                    // search cache for 
                    key = (key_type) strdup(tokens[1].c_str());
                    uint32_t val_size;
                    val = cache_get(cache, key, &val_size);

                    if (val != NULL) {
                        std::string body(tokens[1]);
                        body.append(" ");
                        body.append(std::string((char*) val));
                        send_ok(body);
                    } else {
                        std::cout << "val was NULL" << std::endl;
                        send_bad_request();
                    }

                    free((char*) key);
                    free((void*) val); // TODO not sure if this is part of API
                    break;
                case POST:
                    if (request.path.compare("/shutdown") == 0) {
                        send_ok();
                        return true; // true indicates shutdown
                    } else {
                    }
                    break;
                case PUT:
                    tokens = string_split(request.path, '/');

                    // TODO check for bad input

                    // weird because cache is written in C
                    key = (key_type) strdup(tokens[1].c_str());
                    val = (val_type) strdup(tokens[2].c_str());
                    val_size = tokens[2].size() + 1;

                    cache_set(cache, key, val, val_size);

                    free((char*) key);
                    free((void*) val); 
                    send_ok();
                    break;
                case DELETE:
                    break;
                case HEAD:
                    break;
                default:
                    std::cout << "hit else statement" << std::endl;
                    send_bad_request();
                    break;
            }
            return false;
        }

        void send_ok() {
            network_server.send("HTTP/1.1 200 OK\n");
        }

        void send_ok(std::string body) {
            std::string response = "HTTP/1.1 200 OK\n";
            response.append(body);
            response.append("\n");
            network_server.send(response);
        }

        void send_bad_request() {
            network_server.send("HTTP/1.1 400 Bad Request\n");
        }

};
