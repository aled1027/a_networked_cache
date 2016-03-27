#pragma once

#include <boost/asio.hpp>

class NetworkClient 
{
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

        void request(std::string method, std::string path, std::string version) {
            std::string request = method;
            request.append(" ");
            request.append(path);
            request.append(" ");
            request.append(version);
            request.append("\n");
            network_client.send(request);
            std::string raw_response = network_client.receive();
            http_info response = parse_response(raw_response);
            print_http(response);
        }

};

