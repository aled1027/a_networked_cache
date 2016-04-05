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
#include "poco_server.h"

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/MulticastSocket.h"
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/DatagramSocket.h>
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"

int send() {
    std::cout << "send" << std::endl;
    Poco::Net::SocketAddress sa("localhost", 8081);
    Poco::Net::DatagramSocket dgs;
    dgs.connect(sa);
    Poco::Timestamp now;
    std::string msg = Poco::DateTimeFormatter::format(now,
            "<14>%w %f %H:%M:%S Hello, world!");
    dgs.sendBytes(msg.data(), msg.size());
    std::cout << "sent" << std::endl;
    return 1;
}

int main(int argc, char* argv[]) {
    assert(argc == 2);
    if (argv[1][0] == 's') {
        Poco::Net::SocketAddress sa_recv("localhost", 8082);
        Poco::Net::DatagramSocket dgs_recv(sa_recv);

        Poco::Net::SocketAddress sa_send("localhost", 8081);
        Poco::Net::DatagramSocket dgs_send;
        dgs_send.connect(sa_send);

        Poco::Timestamp now;
        std::string msg = Poco::DateTimeFormatter::format(now,
                "<14>%w %f %H:%M:%S Hello, world!");
        dgs_send.sendBytes(msg.data(), msg.size());
        std::cout << "server sent" << std::endl;

        Poco::Net::SocketAddress sender;
        char buffer[2048];
        int n = dgs_recv.receiveFrom(buffer, sizeof(buffer)-1, sender);
        buffer[n] = '\0';
        std::cout << sender.toString() << ": " << buffer << std::endl;
        std::cout << "server received" << std::endl;

    } else {
        Poco::Net::SocketAddress sa_recv("localhost", 8081);
        Poco::Net::DatagramSocket dgs_recv(sa_recv);

        Poco::Net::SocketAddress sa_send("localhost", 8082);
        Poco::Net::DatagramSocket dgs_send;
        dgs_send.connect(sa_send);

        char buffer[2048];
        while (true) {
            Poco::Net::SocketAddress sender;
            int n = dgs_recv.receiveFrom(buffer, sizeof(buffer)-1, sender);
            buffer[n] = '\0';
            std::cout << sender.toString() << ": " << buffer << std::endl;

            Poco::Timestamp now;
            std::string msg = Poco::DateTimeFormatter::format(now,
                    "<14>%w %f %H:%M:%S Hello, world!");
            dgs_send.sendBytes(msg.data(), msg.size());
            std::cout << "client sent" << std::endl;
        }
    }

    //Server s;
    //s.start();
    //cache_tests();
    //network_tests(argv[1]);
    //go();


    return 0;    
}
