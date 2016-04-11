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

# Benchmarking

## 1. State goals and define system
The goal of this exercise is to assess the throughput of the cache system.
Our system that are we are assessing is limited to the code that we wrote:
1. The server configuration
2. The high level server code, so not assessing TCP, HTTP, or the implementation of HTTP since Poco does this.
3. We aim to test the performance of all of our code, but in particular `poco_server.c`, `cache.c` and `evict.c`
4. Not the operating system or anything below.
5. Our performance evaluation will implicitly include networking speed, but that is not our focus.

## 2. List services and outcome
1. We use a networking service via Poco, HTTP, TCP and UDP. We are not testing the reliability of these services, and in particular, UDP may fail.

## 3. Select metrics
We will measure sustained throughput, defined as the maximum offered load (in requests per second) at which the mean response time remains under 1 millisecond.

## 4. List parameters
#### System Parameters
1. 
#### Workload Parameters
1. Distribution of requests
2. Frequency of requests
3. Key size and nature of keys of requests
4. Value size and nature of values of requests


## 5. Select factors to study

## 6. Select evaluation technique

## 7. Select workload

## 8. Design experiment

## 9. Analyze and interpret data

## 10. Present results



