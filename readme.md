# Let's network
A networked look-aside cache that can be access asynchronously by multiple clients.

## Usage
1. Install cmake
2. Install boost
3. Install poco
4. `cmake .` will generate makefile
5. `make` will compile our code and link libraries as needed
6. In one terminal, call `make run_server`
7. In another terminal, call `make run_client`
8. Watch it go

## Contributing
If a new cpp file is added, it needs to be added to CMakeLists.txt.
Where should be clear when you look at the file.
Then run `cmake .` to recreate the makefile.

