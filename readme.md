# Let's network
A networked look-aside cache that can be access asynchronously by multiple clients.

- sudo perf record ./asdf server
- sudo perf report

## TODO
- run valgrind
- get baseline performance numbers
- change key and valyue generation to uniform
    - Eitan said in class that normal isn't as accurate
- threading
    - throw lock around entire evict struct
    - necessary parts of cache
- ideas that Eitan mentioned for improving cache:
    - a lot of cache misses
    - hash function called alot
   

## Usage
1. Install cmake
2. Install boost
3. Install poco
4. `cmake .` will generate makefile
5. `make` will compile our code and link libraries as needed
6. In one terminal, call `make run_server`
7. In another terminal, call `make run_client`
8. Watch it go

## How to run benchmarking script
Currently, we have a python script, `performance_tests.py` which does the brunt of the heavy lifting for our benchmark tests. 
If you want to run the tests yourself, you need to have a few python libraries installed. 
The file `requirements.txt` lists them. To install, (ideally) activate a new virtual environment and then `pip install -r requirements.txt`. 

## Contributing
If a new cpp file is added, it also needs to be added to `CMakeLists.txt`.
Where should be clear when you look at the file.
Then run `cmake .` to recreate the makefile.

# Multicache

## Threading
Poco automatically multithreads makes the TCP server. 
Poco uses a thread pool and assigns incoming connections to a thread.
The documentations states that the number of threads used is adjusted dynamically depending on the number of connections
waiting; however, w
For more information on Poco's TCP server multithreading, see the Poco docs on TCPServer.

Our UDP server, which we wrote using Poco's datagram socket, is not automatically multithreaded.  

## Locking
In order to lock, we used Poco's FastMutex mutex and a ScopedLock.
As the name suggests, a scoped lock locks a mutex for the scope, and releases the lock at the end of the scope.

We have three mutexes in our code.
One is in the eviction object, where all evict methods, except `create_evict`, lock at the beginning of the method and
unlock at the end.

A second mutex is in the hash function; it's called `hash_mutex`.
Not sure why I put this there right now, but in the back of my mind, I remember you, Eitan, suggesting that we put a mutex there.

The final mutex is on the cache, and protects necessary cache operations; the mutex is called `cache_mutex`.
Specifically, the cache mutex locks xxx yyy zzz TODO.

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
We use a networking service via Poco, HTTP, TCP and UDP. We are not testing the reliability of these services, and in particular, UDP may fail.
Tests done on Alex's computer use the Linux OS and tests on Isabella's computer use MacOS.

## 3. Select metrics
We will measure sustained throughput, defined as the maximum offered load (in requests per second) at which the mean response time remains under 1 millisecond.

## 4. List parameters

#### System Parameters
1. Hash function
2. Setting `maxmem`, max load factor, reset load factor
3. Eviction policy (fixed to LRU)

#### Workload Parameters
1. Request rate
2. Read/write ratio
3. Key size distribution
4. Value size distribution
5. Key reuse period
6. Inter-arrival gap

## 5. Select factors to study

#### Those that will be varied
1. Sustained throughput (in requests per second)

#### Those that will not vary
1. Read/write ratio
2. Key size distribution
3. Value size distribution
4. Key reuse period
5. Inter-arrival gap
5. Eviction policy
6. Hash function
7. Load factor parameters
8. The initial `maxmem`

Key size, value size and inter-arrival time gap are fixed but nonconstant (they will be modeled with distributions)

## 6. Select evaluation technique
We have three choices for evaluation technique: analytical modeling, simulation and measuring a real system. 
We are doing a simulation because (1) analytical modeling would give little information about our brand new cache and (2) we do not have access to a real system. 
This leaves us to run a simulation.

## 7. Select workload
In our simulation, we aim to mimic the workload of the ETC pool of Facebook's memcached.
We model key size with a poisson distribution with mean 30. 
We model value size with an normal distribution with mean 100 and standard deviation 8
We model the inter-arrival gap to be constant dependent on the number of transactions per second.

We note that Facebook observes that ETC's value size and inter-arrival gap have a long-tail distribution, which we do not capture by using normal distributions.
We chose to use normal distributions because normal distributions are accessible and felt that a normal distribution was sufficient.
If a cache is being used correctly, then there shouldn't be too many large values. 
This is an arguable experimental design decision.

## 8. Design experiment
Used mutilate for distributions:  [https://github.com/leverich/mutilate](https://github.com/leverich/mutilate)
As a setup step, we randomly generate some (key, value) pairs and put them in the cache. When sending a request, we randomly choose a key and a value (not necessarily the same pair) and pass that information to the correct request type we are attempting to make. Note that the only request type which requires a value is the UPDATE (put) request type, since we wish to replace the value. 

At first, the idea behind the experimental design was to create a thread which sent GET requests at a certain rate for a sustained amount of time (30 seconds). This was done by having two threads: one responsible for sending the request, grabbing a timestamp, and incrementing a global variable; and a second thread responsible for listening to responses to those requests and keeping track of the analgous data. 

However, this presented a problem when we attempted to simulate the ETC workload from the memcached paper. Namely, we chose to use python requests library to help us send PUT, UPDATE, and DELETE requests to our cache. The python requests library is not asychronous, so it is proving to be quite difficult to send requests at a desired rate when the workload is varied. 

To solve this we need to do (at least) two things:
  * Figure our which library would best suit our needs and integrate it into `workload.py`
  * Modify the relevant global variables so that they are either no longer global, or can only be modified after aquiring a lock. 

In the meantime, we are using a Timer() object to allow us to send PUT, UPDATE, and DELETE requests in a way that's non-blocking. However, this does nothing to avoid the potential race conditions when it comes to updating global variables.

## 9. Analyze and interpret data
asdf

## 10. Present results
Due to the issues outlined in section 8 (Design experiment), a workload is currently composed entirely of get requests. The issue with the workload is compounded by the fact that our cache is not currently thread-safe, though it does use multiple threads. When we attempt to simulate a mixed workload on our cache, the server will often encounter a memory allocation or freeing error, which is fatal and prevents data from being collected on the client-side.

We note that for a workload of 100% GET requests, the mean response time begins to exceed 1ms at approximately 3000requests per second. Past this point, we notice a dramatically increased response time (an order of magnitude increase between 3000req/sec and 3250 req/sec) and a marked increase in packet loss. At a rate of 10000req/sec, we see that the mean response time is (WHAT???? thoughts, alex?) and we've received under 50% of our expected responses.

## Misc notes
- Added a bit of crap to `poco_server.cpp` to `MyUDPServer::runTask` to allow python sockets to work.
- I couldn't get a udp socket to go to `localhost:port/key`, only to `localhost:port` with a message.
- should be a way to it, but wasn't working for me.




