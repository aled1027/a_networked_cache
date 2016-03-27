/**
 * tcpIpCs.cpp
 *
 * http://thisthread.blogspot.com/2011/02/minimal-asio-tcp-server.html
 * http://thisthread.blogspot.com/2011/02/minimal-asio-tcp-client.html
 *
 * http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/tutorial/tutdaytime1.html
 * http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/tutorial/tutdaytime2.html

 */

#include <iostream>
#include <exception>
#include <array>
#include <boost/asio.hpp>
#include "network_server.h"

//#define DEBUG

void debug(std::string s) 
{
#ifdef DEBUG
    std::cout << s << std::endl;
#endif
}

namespace globals
{
    const int HELLO_PORT = 50013;
    const char* HELLO_PORT_STR = "50013";
    const char* HOST = "localhost";
}


class Server 
{
    public:
        boost::asio::io_service io;
        boost::asio::ip::tcp::socket socket;
        boost::asio::ip::tcp::acceptor acceptor;
        Server() :
            socket(io),
            acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), globals::HELLO_PORT)) {
                acceptor.accept(socket); // wait and listen
                debug("Server::Server");
            }
        ~Server() {
            debug("Server::~Server");
        }

        void receive() {
            boost::asio::streambuf sb;
            boost::system::error_code ec;
            while (boost::asio::read(socket, sb, ec)) {
                std::cout << &sb << "'\n";
            }
        }

        void send(const std::string& message) {
            debug("Server::send");
            try {
                //acceptor.accept(socket); // wait and listen
                boost::asio::write(socket, boost::asio::buffer(message));
                acceptor.close();
            } catch(std::exception& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
            }
        }
};

class Client 
{
    public:
        boost::asio::io_service io;
        boost::asio::ip::tcp::resolver resolver;
        boost::asio::ip::tcp::socket socket;
        Client() :
            resolver(io),
            socket(io)
    {
        debug("Client::Client");
        try {
            boost::asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(
                    boost::asio::ip::tcp::resolver::query(globals::HOST, globals::HELLO_PORT_STR));
            boost::asio::connect(socket, endpoint);
        } catch(std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
        ~Client() {
            debug("Client::~Client");
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

int main(int argc, char* argv[])
{
    // https://raw.githubusercontent.com/egalli64/thisthread/master/asio/tcpIpCs.cpp
    // http://thisthread.blogspot.com/2013/09/simple-asio-tcp-clientserver-example.html
    assert(argc == 2);
    std::string client_str("client");
    if (client_str.compare(argv[1]) == 0) {
        Client c;
        std::string message("Hello from Alex");
        c.send(message);
        c.send(message);
    } else {
        Server s;
        std::string message("Hello from Alex");
        s.receive();
    }

}
