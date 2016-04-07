# Let's network
A networked look-aside cache that can be access asynchronously by multiple clients.

## Question 1
For this homework, we elected to use and learn a few technologies: C++, CMake, Boost (has been removed, but was in the code for a while) and Poco. 
We decided to use C++ because it is more often requested by employers and seemed to have healthier libraries - and it lead to cleaner to code. 
We decided to use CMake because Alex had run across quite a few projects that use it and wanted to become familiar with it. It turned out to be quite useful.
And finally, we ended up using Poco to manage our networking code (and we used it parse JSON).

You may find our server-side code in files `include/poco_server.h` and `src/poco_server.cpp`.
You will find that at the highest layer of abstraction is a class `Server`, which has no member variables, only a method `start()`. 
`Server::start()` starts the TCP server, and if neccessary, opens the UDP socket. 
The TCP server is implemented with the class `MyTCPServer`, which inherits from a Poco class `ServerApplication`. 
`MyTCPServer` overrides the method `MyTCPServer::main()`, which starts the HTTPServer provided by Poco. 
The HTTPServer handles requests via `MyRequestHandler`.
All requests are piped through the method `MyRequestHandler::handleRequest`, which calls the appropriate function based on the method type. 

The Poco code handles pretty much everything behinds the scenes.
It runs HTTP via TCP, and gives us a `HTTPServerRequest` object. 
We retrieve information from the `HTTPServerRequest` object, perform the necessary operations, and fill a `HTTPServerResponse` which is sent back to the client.
It's extremely nice. 
The biggest pitfall was that the HTTPServer always uses TCP; we couldn't easily get it also use UDP, so we did that from scratch, so to speak.

The UDP server is implemented via a Poco class called a `DatagramSocket`. 
We simply listen for messages on the socket, and respond as needed. 
As the problem specifies, we only use UDP for get requests, but we do not use HTTP. 
Adding in HTTP required us to parse the requests and responses, and added a bunch of work that we simply didn't have time to do. 
Despite this, the GET requests are still RESTful - there is not statefulness - they just don't use HTTP.

The client side is also implemented using Poco, see files `include/poco_client.h` and `src/poco_client.cpp`.
All of the networking code is essentially inside of `Client::send`. 
`Client::send` takes as input information about the HTTP request, it then sends the request to the server, and forwards to response to the method that called it. 

The UDP client code is `Client::cache_get`. 
It simply opens up a datagram socket, sends the key, and gets pack json information about the key and value. 

TODO TALK ABOUT TIMINGS

## Caveats
We believe that there are some bugs present in the code. We did not get the chance fix errors from flags, and we did not get a chance to run valgrind and find memory leaks. 

Moreover, the code is *threaded*. Poco uses 4 asynchrononous threads for the HTTP-TCP server, and we added a fifth thread for the UDP server. Given our limited knowledge of threading, we suspect that this could lead to issues, and we did nothing to alleviate them. That being said, we never came across an error that seemed to be due to parallelism at any point, so maybe Poco implements something behind the scenes to deal with this issue. The parallelism is something that we will consider more strongly in the parallelism homework. 

Finally, we did not add in command line arguments, due to lack of time.
However, the system is easily customizable by changing the value of variables in `src/globals.cpp`. 

## Usage
1. Install cmake
2. Install boost
3. Install poco
4. `cmake .`
5. `make`
6. In one terminal, `make run_server`
7. In another terminal, `make run_client`
8. Watch it go

## Contributing
If a new cpp file is added, it needs to be added to CMakeLists.txt.
Where should be clear when you look at the file.
Then run `cmake .` to recreate the makefile.

##Testing
  * change globals::USE_UDP to true
    * In one terminal, `make run_server`
    * In one terminal, `make run_client` 
    * output on client-side confirms tests and shows the results of timing 1000 get requests with UDP
      * Avg time on @ifjorissen's MBP: .63ms
  * change globals::USE_UDP to false
    * In one terminal, `make run_server`
    * In one terminal, `make run_client` 
    * output on client-side confirms tests and shows the results of timing 1000 get requests with TCP
      * Avg time on @ifjorissen's MBP: .9ms

## New things we are trying
- cplusplus
- cmake
- boost/asio

## Resources
- threading / tasks in poco
    - http://stackoverflow.com/questions/9909901/stopping-a-pocothread-for-unit-tests
- https://www.jmarshall.com/easy/http/#whatis
- https://github.com/eidheim/Simple-Web-Server/blob/master/http_examples.cpp
- http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/examples/cpp11_examples.html
- http://stackoverflow.com/questions/15290386/using-boost-asio-with-cmake
- http://think-async.com/Asio/AsioAndBoostAsio
    - https://github.com/chriskohlhoff/asio
    - examples for CPP14 look pretty good
- http://stackoverflow.com/questions/118945/best-c-c-network-library
- https://github.com/google/cpp-netlib
- ima try using cmake for the makefile
    - https://cmake.org/

```
$ nc -l localhost 8000 &
[1] 3150
$ curl --proxy localhost:8000 --silent --max-time 1 http://www.stackoverflow.com
GET http://www.stackoverflow.com HTTP/1.1
User-Agent: curl/7.19.7 (x86_64-pc-linux-gnu) libcurl/7.19.7 OpenSSL/0.9.8k zlib/1.2.3.3 libidn/1.15
Host: www.stackoverflow.com
Accept: */*
Proxy-Connection: Keep-Alive

[1]+  Done                    nc -l localhost 8000
```
