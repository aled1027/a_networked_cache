#pragma once

#include <boost/asio.hpp>

class NetworkClient 
{
    public:
        boost::asio::ip::tcp::resolver resolver; // make io_service use tcp
        boost::asio::ip::tcp::socket socket;

        NetworkClient(boost::asio::io_service& io) : resolver(io), socket(io) {
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

        void receive() {
            debug("NetworkClient::receive");
            boost::asio::streambuf sb;
            boost::system::error_code ec;
            std::stringstream ss;
            boost::asio::read_until(socket, sb, "\n", ec);
            std::cout << &sb;
        }
};


