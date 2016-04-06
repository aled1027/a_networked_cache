# Let's network
A networked look-aside cache that can be access asynchronously by multiple clients.

## Getting started
1. Install cmake
2. Install boost
3. Install poco
4. `cmake .`
5. `make`
6. In one terminal, `make run_server`
7. In another terminal, `make run_client`

## Contributing
If a new cpp file is added, it needs to be added to CMakeLists.txt.
Where should be clear when you look at the file.
Then run `cmake .` to recreate the makefile.

##Testing
  * change globals::USE_UDP to true
    * In one terminal, `make run_server`
    * In one terminal, `make run_client` 
    * output on client-side confirms tests and shows the results of timing 1000 get requests with UDP
      * Avg time on @ifjorissen's MBP: 1.24ms
  * change globals::USE_UDP to false
    * In one terminal, `make run_server`
    * In one terminal, `make run_client` 
    * output on client-side confirms tests and shows the results of timing 1000 get requests with TCP
      * Avg time on @ifjorissen's MBP: .8ms

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
