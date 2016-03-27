#pragma once

#include <boost/asio.hpp>

class NetworkClient 
{
    public:
        boost::asio::io_service io;
        boost::asio::ip::tcp::resolver resolver;
        boost::asio::ip::tcp::socket socket;

        NetworkClient() :
            resolver(io),
            socket(io)
    {
        debug("NetworkClient::NetworkClient");
        try {
            boost::asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(
                    boost::asio::ip::tcp::resolver::query(globals::HOST, globals::HELLO_PORT_STR));
            boost::asio::connect(socket, endpoint);
        } catch(std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
        ~NetworkClient() {
            debug("NetworkClient::~NetworkClient");
        }

        void send(const std::string& message) {
            socket.send(boost::asio::buffer(message));
        }

        void receive() {
            debug("Client::receive");
            try {
                std::array<char, 40> buf;
                boost::system::error_code error;
                size_t len = socket.read_some(boost::asio::buffer(buf), error);

                if(error == boost::asio::error::eof)
                    return; // connection closed by peer?

                else if(error)
                    throw boost::system::system_error(error);
                std::cout.write(buf.data(), len);
                std::cout << std::endl;
            } catch(std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        }
};


