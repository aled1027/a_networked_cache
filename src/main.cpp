/**
 * tcpIpCs.cpp
 * http://thisthread.blogspot.com/2011/02/minimal-asio-tcp-server.html
 * http://thisthread.blogspot.com/2011/02/minimal-asio-tcp-client.html
 * http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/tutorial/tutdaytime1.html
 * http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/tutorial/tutdaytime2.html
 * https://raw.githubusercontent.com/egalli64/thisthread/master/asio/tcpIpCs.cpp
 * http://thisthread.blogspot.com/2013/09/simple-asio-tcp-clientserver-example.html
 *
 * http://www.boost.org/doc/libs/1_35_0/doc/html/boost_asio/tutorial/tutdaytime1.html
 */

#include <iostream>
#include <exception>
#include <array>
#include <boost/asio.hpp>
#include "globals.h"
#include "network_server.h"
#include "network_client.h"

int main(int argc, char* argv[])
{
    assert(argc == 2);
    std::string client_str("client");
    if (client_str.compare(argv[1]) == 0) {
        boost::asio::io_service io_service;
        NetworkClient c(io_service);
        c.send("GET /path/to/file/index.html HTTP/1.0\n");
    } else {
        boost::asio::io_service io_service;
        NetworkServer s(io_service);
        s.receive();
    }

}
