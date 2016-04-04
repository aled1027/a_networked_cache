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

#include <assert.h>
#include "cache_tests.h"
#include "network_tests.h"


int main(int argc, char* argv[]) {
    assert(argc == 2);
    //cache_tests();
    network_tests(argv[1]);

    return 0;    
}
