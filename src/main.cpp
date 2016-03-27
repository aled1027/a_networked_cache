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
#include "globals.h"
#include "network_server.h"
#include "network_client.h"


int main(int argc, char* argv[])
{
    // https://raw.githubusercontent.com/egalli64/thisthread/master/asio/tcpIpCs.cpp
    // http://thisthread.blogspot.com/2013/09/simple-asio-tcp-clientserver-example.html
    assert(argc == 2);
    std::string client_str("client");
    if (client_str.compare(argv[1]) == 0) {
        NetworkClient c;
        std::string message("Hello from Alex");
        c.send(message);
        c.send(message);
    } else {
        NetworkServer s;
        std::string message("Hello from Alex");
        s.receive();
    }

}
