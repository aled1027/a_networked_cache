#pragma once

class HTTPServer
{
    public:
        NetworkServer network_server;

        HTTPServer() 
}

class NetworkServer 
{
    public:
        boost::asio::io_service io;
        boost::asio::ip::tcp::socket socket;
        boost::asio::ip::tcp::acceptor acceptor;
        NetworkServer(boost::asio::io_service& io) :
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

        void receive() {
            debug("NetworkServer::receive");
            boost::asio::streambuf sb;
            boost::system::error_code ec;
            boost::asio::read_until(socket, sb, "\n", ec);
            std::cout << &sb;
        }

        void send(const std::string& message) {
            debug("NetworkServer::send");
            try {
                boost::system::error_code ignored_error;
                boost::asio::write(socket, boost::asio::buffer(message), boost::asio::transfer_all(), ignored_error);
            } catch(std::exception& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
            }
        }
};


