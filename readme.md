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

## How to run benchmarking script
Currently, we have a python script, `performance_tests.py` which does the brunt of the heavy lifting for our benchmark tests. 
If you want to run the tests yourself, you need to have a few python libraries installed. 
The file `requirements.txt` lists them. To install, (ideally) activate a new virtual environment and then `pip install -r requirements.txt`. 

## Contributing
If a new cpp file is added, it also needs to be added to `CMakeLists.txt`.
Where should be clear when you look at the file.
Then run `cmake .` to recreate the makefile.

## FAQ Checklist
1. Do you have -Og? clang doesn't -Og.
1. Are you compiling with boost?
3. Is `globals::IS_PYTHON_CLIENT` set correctly?
4. Is `globals::USE_UDP` set correctly?
5. Are `globals::HOST` and the client-side host set correctly?
6. Too many evictions? Check default `globals::MAXMEM`.

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
As a setup step, we randomly generate some (key, value) pairs and put them in the cache. When sending a request, we randomly choose a key and a value (not necessarily the same pair) and pass that information to the correct request type we are attempting to make. Note that the only request type which requires a value is the UPDATE (put) request type, since we wish to replace the value. 

At first, the idea behind the experimental design was to create a thread which sent GET requests at a certain rate for a sustained amount of time (30 seconds). This was done by having two threads: one responsible for sending the request, grabbing a timestamp, and incrementing a global variable; and a second thread responsible for listening to responses to those requests and keeping track of the analgous data. 

However, this presented a problem when we attempted to simulate the ETC workload from the memcached paper. Namely, we chose to use python future-requests library to help us send PUT, UPDATE, and DELETE requests to our cache. future-requests is asynchronous, so we add requests to a queue and then when the timing period is over, we empty the queue, attempting to read the result from the future object. 

A future request is considered to be *lost* if the request takes more than 25ms to complete; however, the requests do not time out until 6 seconds pass. This is because we can only figure out the type of response that timed out (or was lost) if we can wait for the task itself to complete. Note that we count the elapsed time of the "lost" requests in our cache, so our mean put / delete requests is higher due to that decision. 

## 9. Analyze and interpret data

Since the TCP portion of our server is already mulithreaded, we also looked at the difference resulting from having multiple threads on the UDP portion. 

We sent requests to the server at varying rates between 50 and 1000 requests per second. We sustained these rates for 15 seconds, at which point we looked at the the number of each type of request, mean times for each type of request, and number of packets lost. The raw data can be found in `data/`. 

Interpreting the graph: the red marked line shows the mean GET response time for a given rate (listed on the x-axis). The blue marked line is the average mean reponse time across all types of requests (GET, PUT, DELETE). The green marked line indicated the mean response time for PUT requests, and the teal line is the mean elapsed time for a DELETE request. 

Singly threaded UDP:
![singly threaded udp (pre optimization)](https://github.com/aled1027/a_networked_cache/blob/master/data/pre_optimize_1UDP.png "Singly threaded UDP")

This chart suggests that the mean GET response time exceeds 1ms when we reach approximately 500 requests per second. 

UDP with two:
![singly threaded udp (pre optimization)](https://github.com/aled1027/a_networked_cache/blob/master/data/pre_optimize_2UDP.png "2 threaded UDP")


UDP with 4 threads:
![singly threaded udp (pre optimization)](https://github.com/aled1027/a_networked_cache/blob/master/data/pre_optimize_4UDP.png "4 threaded UDP")


UDP with 8 threads:
![singly threaded udp (pre optimization)](https://github.com/aled1027/a_networked_cache/blob/master/data/pre_optimize_8UDP.png "8 threaded UDP")

[finish this!]


## 10. Present results
Due to the issues outlined in section 8 (Design experiment), a workload is currently composed entirely of get requests. The issue with the workload is compounded by the fact that our cache is not currently thread-safe, though it does use multiple threads. When we attempt to simulate a mixed workload on our cache, the server will often encounter a memory allocation or freeing error, which is fatal and prevents data from being collected on the client-side.

We note that for a workload of 100% GET requests, the mean response time begins to exceed 1ms at approximately 3000 requests per second. Past this point, we notice a dramatically increased response time (an order of magnitude increase between 3000req/sec and 3250 req/sec) and a marked increase in packet loss.

## Misc notes
- Added a bit of crap to `poco_server.cpp` to `MyUDPServer::runTask` to get python sockets to work.
- I couldn't get a udp socket to go to `localhost:port/key`, only to `localhost:port` with a message.
- should be a way to do it, but wasn't working for me.
- same as `echo -n "key" | nc -4u -w1 localhost 8081`

# Multicache

## MultiThreading
Poco automatically multithreads the TCP server. 
Poco uses a thread pool and assigns incoming connections to a thread.
The documentations states that the number of threads used is adjusted dynamically depending on the number of connections
waiting; however, I observe that it generally only uses a single thread since most of our workload is UDP-gets.
I did not check to see at what threshold the TCP server jumps to a second or third thread.
For more information on Poco's TCP server multithreading, see the Poco docs on TCPServer.

Our UDP server, which we wrote using Poco's datagram socket, is not automatically multithreaded. 
We multithread our UDP server by opening up many UDP servers on multiple threads using Poco's task manager.
Task manager takes as input a unique pointer (I think, or something like it), and it runs and manages the thread.
Task manager has the nice feature where we can easily kill the threads when we decide to shutdown the server.
The UDP multithreading code can be found at `src/poco_server.cpp::Server::start()`.

The number of UDP threads is variable - the value is set in `globals.cpp`. See HW6 for how the number of UDP threads affects performance.

## Locking
We used Poco's `FastMutex::mutex` and `FastMutex::ScopedLock` to provide locking functionality.
As the name suggests, a scoped lock locks a mutex for the scope, releasing the lock at the end of the scope.
This is the recommended way to use Poco locks, according to the docs.

We have two mutexes in our code.
One is in the eviction object, where all evict methods, except `create_evict`, lock at the beginning of the method and unlock at the end.

The other mutex is in the cache, and it protects necessary cache operations; this mutex is called `cache_mutex`.
The cache mutex locks `cache_set`, `cache_delete`, `cache_get` and `cache_destroy`. 
We note that `cache_dynamic_resize` is not locked, as it is only called from `cache_set`, so the thread should own the mutex when `cache_dynamic_resize` is called; we added an assert to ensure this is the case.
On a similar note, `cache_delete` is called from two places: inside of `cache_set` and from the public API.
In order to prevent a deadlock, I (Alex) broke the DRY rule and made a new function called `cache_unsafe_delete` which is the same as code as `cache_delete` without the lock.
The function `cache_unsafe_delete` is only called from `cache_set`, is not accessible in the public API, and has an assert at the top to check that the thread owns the mutex.

# Crank It Up 

## Step 1
The elapsed time for a single get request before optimization was below 1ms. 

## Step 2
Isabella's analysis here.

## Step 3
I devoted some time in optimizing performance to seeing how time is spent in a single get request.
My thinking was that if we can optimize the performance of a single UDP get request, then our server can handle more requests per second.
It was also easier to check the performance of a single request: I could send the request with curl or netcat and add statements that print timings to the c++ code. 

To this end, I added statements in `poco_server.cpp:MyUDPServer::runTask()` to measure the time, in nanoseconds, it takes to complete each step in processing a UDP request.
According to the print statements on the server side, a UDP get request, from receiving a message to sending a message off, ranges from taking between 0.15 and 0.4 ms, with a high variance in that window, but not exceeding it or dropping below it.
Greater than 95% of the time is spent on the lines:
```
n = dgs.receiveFrom(buffer, sizeof(buffer)-1, sender);
buffer[n] = '\0';
```
The Poco docs do not say, but (with my naive understanding of networking) I expect that `dgs.receiveFrom` checks the UDP queue held by the OS to see if there are packets waiting to be processed.
If there are packets to be processed, then `dgs.receiveFrom` collects the data, stores it in buffer, and returns the size of the buffer.

The time that we are really concerned about is the moment that the UDP packet enters the queue, to the time that `dgs.receiveFrom` returns.
It is possible that `dgs.receiveFrom` sits at the queue for some amount of time waiting for a packet to arrive.
If this is the case, then the time that `dgs.receiveFrom` takes does accurate not accurately reflect how `dgs.receiveFrom` contributes the time spent processing a client's request.

We can, however, glean some information.
According the our timings found with our python script, we know that an average UDP get request takes around 0.25 ms (on Isabella's computer, longer on mine - closer to 0.35ish ms). 
According to my print statements in `poco_server.cpp::MyUDPServer::runTask()`, all lines (including `cache_get`) except for `dgs.receiveFrom` take approximately 0.01 ms.
The time suck is therefore either occuring in the function `dgs.receiveFrom` or somewhere in the networking or networking layer.
The unaccounted time could be occuring in the communication/networking, but because we are running these tests over localhost, the networking time is minimal.

Based on this information, we felt that it was unwise to further improve our cache code, as our print statements suggest that it is not a major bottleneck; in fact, they suggest the opposite: the cache is quite fast.
The print statements say that calling `cache_get` takes rougly 0.005 ms.
I should note that all timings are measuring the performance of the cache on a single request and I am running the server with a single UDP thread, such that multithreading is not used, and there is no locking.
We have some locks on our cache, so this time will be slower if the UDP server receives multiple requests at once.

Future work on this project would include investigating Poco's datagram implementation; in particular, timing it in isolation to see its performance. 
Poco might be fast, and something else is being slow around `dgs.receiveFrom`, or Poco is slow, and we either use a different library for UDP or write our own. 

## Optimizations before timings
Before we finished our timing script, and before I did the analysis in the previous subsection, I implemented some optimizations based on the output of our perf report.
The perf report gave very flat timings - everything was less than 2% - which I now know means that I should have removed `-Og` or something else to make the report more accurate (I was using sudo).
But I didn't do those things because I didn't know at the time.

In the perf report, I observed that `evict_get` uses the most time of any `a_networked_cache` functions, and the other eviction functions took a disproportionate amount of time as well.
For context, `evict_get` was taking 0.69% of the time.
I zoomed in on `evict_get` and saw that most of the time, according to Perf, was spent on an if statement in the middle of a loop that checks if a pointer is NULL.
We had a naive implementation of LRU, which I believe was causing many branch misses (Perf verified we had a lot of branch misses - unsure where), and our implementation required us to access many pointers that are in random places in memory in every operation.

I decided to change our eviction policy to evict a random object in the cache.
I implemented this using `std::set`.
Essentially, we have a `std::set` of all objects in the cache, and we select an element at random from the set to evict.
I ran Perf after this implementation, and all evict operations dropped to less than 0.08% of the time.

I also observed that many of the functions at the top of the perf report were related to threading and locking.
Since for homework 7 I threw locks on pretty naively, I decided to look through and see what could be changed.
I put all calls to our hash function outside of locks (our hash function was relatively high on the Perf list).
I put as many calls to the eviction policy outside of the cache locks since the eviction object has its own locks.
Finally, our if-condition that checks if the cache needs to resized outside of the function `cache_dynamic_resize` and right into the function (inlined it, so to speak).
Since `cache_dynamic_resize` is called so rarely, we save a function call - this change moved `cache_dynamic_resize` way down on the perf report.

## Misc Notes on HW8
- To use perf:
  - sudo perf record ./asdf server
  - sudo perf report
