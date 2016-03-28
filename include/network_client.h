#pragma once

#include <boost/asio.hpp>
#include "globals.h"

class NetworkClient {
    public:
        boost::asio::io_service io;
        boost::asio::ip::tcp::resolver resolver; // make io_service use tcp
        boost::asio::ip::tcp::socket socket;

        NetworkClient() : resolver(io), socket(io) {
            debug("NetworkClient::NetworkClient");
            boost::asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(
                    boost::asio::ip::tcp::resolver::query(globals::HOST, globals::HELLO_PORT_STR));

            boost::system::error_code error = boost::asio::error::host_not_found;
            socket.connect(*endpoint, error);

            if (error) {
                throw boost::system::system_error(error);
            }
        }

        ~NetworkClient() {
            debug("NetworkClient::~NetworkClient");
        }

        void send(const std::string& message) {
            debug("NetworkClient::send");
            socket.send(boost::asio::buffer(message));
        }

        std::string receive() {
            debug("NetworkClient::receive");
            boost::asio::streambuf sb;
            boost::system::error_code ec;
            boost::asio::read_until(socket, sb, "\n", ec);
            std::ostringstream ss;
            ss << &sb;
            return ss.str();
        }
};

class HTTPClient {
    public:
        NetworkClient network_client;
        HTTPClient() {
            debug("HTTPClient::HTTPClient");
        }

        ~HTTPClient() {
            debug("HTTPClient::~HTTPClient");
        }

        std::string make_request(std::string method, std::string path) {
            std::string request = "GET";
            request.append(" ");
            request.append(path);
            request.append(" ");
            request.append("HTTP/1.1\n");
            return request;
        }

        std::string get(std::string key) {
            std::string path("/");
            path.append(key);
            network_client.send(make_request("GET", path));

            std::string raw_response = network_client.receive();
            http_info response = parse_response(raw_response);
            uint32_t response_code = std::stoi(response.response_status_code);

            if (response_code != 200) {
                // TODO
            } else {
                // TODO
            }
            return "";
        }

        void set(std::string key, std::string value) {
            std::string path ("/");
            path.append(key);
            path.append("/");
            path.append(value);

            network_client.send(make_request("PUT", path));

        }
        
        void shutdown() {
            network_client.send(make_request("POST", "/shutdown"));
            
        }
};

