# Let's network
A networked look-aside cache that can be access asynchronously by multiple clients.

## Getting started
1. Install cmake
2. `cmake .`
3. `make`
4. In one terminal, `make run_server`
5. In another terminal, `make run_client`

## Contributing
If a new cpp file is added, it needs to be added to CMakeLists.txt.
Where should be clear when you look at the file.
Then run `cmake .` to recreate the makefile.

## New things we are trying
- cplusplus
- cmake
- boost/asio

## Resources
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
