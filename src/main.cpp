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
#include <assert.h>
#include "cache_tests.h"
#include "network_tests.h"


#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/MulticastSocket.h"
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/DatagramSocket.h>
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"

int port = 8080;

int send()
{
    std::cout << "send" << std::endl;
    Poco::Net::SocketAddress sa("localhost", port);
    Poco::Net::DatagramSocket dgs;
    dgs.connect(sa);
    Poco::Timestamp now;
    std::string msg = Poco::DateTimeFormatter::format(now,
            "<14>%w %f %H:%M:%S Hello, world!");
    dgs.sendBytes(msg.data(), msg.size());
    return 0;
}

int recv()
{
    std::cout << "recv" << std::endl;
    Poco::Net::SocketAddress sa("localhost", port);
    Poco::Net::DatagramSocket dgs(sa);
    char buffer[1024];
    for (;;)
    {
        Poco::Net::SocketAddress sender;
        int n = dgs.receiveFrom(buffer, sizeof(buffer)-1, sender);
        buffer[n] = '\0';
        std::cout << sender.toString() << ": " << buffer << std::endl;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    assert(argc == 2);
    if (argv[1][0] == 's') {
        recv2();
    } else {
        send();
    }



    //cache_tests();
    //network_tests(argv[1]);
    //go();


    return 0;    
}
