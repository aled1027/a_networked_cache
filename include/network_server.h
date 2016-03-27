#pragma once

#include <boost/asio.hpp>

class NetworkServer 
{
    public:
        boost::asio::io_service io;
        boost::asio::ip::tcp::socket socket;
        boost::asio::ip::tcp::acceptor acceptor;
        NetworkServer() :
            socket(io),
            acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), globals::HELLO_PORT)) {
                acceptor.accept(socket); // wait and listen
                debug("NetworkServer::NetworkServer");
            }
        ~NetworkServer() {
            debug("NetworkServer::~NetworkServer");
        }

        void receive() {
            debug("NetworkServer::receive");
            boost::asio::streambuf sb;
            boost::system::error_code ec;
            while (boost::asio::read(socket, sb, ec)) {
                std::cout << &sb << "'\n";
            }
        }

        void send(const std::string& message) {
            debug("NetworkServer::send");
            try {
                //acceptor.accept(socket); // wait and listen
                boost::asio::write(socket, boost::asio::buffer(message));
                acceptor.close();
            } catch(std::exception& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
            }
        }
};


