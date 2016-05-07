Current status: udp not receving correct value - only receiving key for some reason??

- Medium term goal: get test.py to find saturation point.



# Let's network
A networked look-aside cache that can be access asynchronously by multiple clients.

- sudo perf record ./asdf server
- sudo perf report
- avg time for 1000: 0.001030 for gets

## TODO
- run valgrind
- get baseline performance numbers
- change key and valyue generation to uniform
    - Eitan said in class that normal isn't as accurate
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

## FAQ Checklist
1. Do you have -Og? clang doesn't -Og.
1. Are you compiling with boost?
3. Is `globals::IS_PYTHON_CLIENT` set correctly?
4. Is `globals::USE_UDP` set correctly?
5. Are `globals::HOST` and the client-side host set correctly?
6. Too many evictions? Check default `globals::MAXMEM`.

## How to run benchmarking script
Currently, we have a python script, `performance_tests.py` which does the brunt of the heavy lifting for our benchmark tests. 
If you want to run the tests yourself, you need to have a few python libraries installed. 
The file `requirements.txt` lists them. To install, (ideally) activate a new virtual environment and then `pip install -r requirements.txt`. 

## Contributing
If a new cpp file is added, it also needs to be added to `CMakeLists.txt`.
Where should be clear when you look at the file.
Then run `cmake .` to recreate the makefile.

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

The number of UDP threads is variable - the value is set in `globals.cpp` - but I usually have it set to use 4 threads.

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

# Crank It Up

## Step 1
Pass

## Step 2
Adapted script from previous homework; see `crank_it_up.py`.

## Step 3
I devoted much of my time in optimizing performance to seeing how time is spent in a single UDP request.
To check this, I added statements in `poco_server.cpp:MyUDPServer::runTask()` to how long, in nanoseconds, it takes to complete various tasks.
The udp operation, from receiving a message to sending a message off, ranges from taking between 0.15 and 0.4 ms, with a high variance in that window, but not exceeding it or dropping below it.
Greater than 95% of the time is spent on the lines:
```
n = dgs.receiveFrom(buffer, sizeof(buffer)-1, sender);
buffer[n] = '\0';
```
The docs do not say, but (with my naive understanding of networking) I expect that `dgs.receiveFrom` checks the UDP queue to see if there are packets waiting to be processed.
If there are packets to be processed, then `receiveFrom` collects the data, stores it in buffer, and returns the size of the buffer.

The time that we are really concerned about is the moment that the UDP packet enters the queue, to the time that the `receiveFrom` acquires the buffer.
It is possible that `receiveFrom` sits at the queue for some amount of time waiting for a packet to arrive.
If this is the case, then the time that `receiveFrom` takes does accurate not accurately reflect how `receiveFrom` contributes the time spent processing a client's request.

We can, however, glean some information.
According the our timings found with our python script, we know that an average UDP get request takes TODOxxx seconds.
According to my print statements in `poco_server.cpp::MyUDPServer::runTask()`, all lines (including `cache_get`) except for `receiveFrom` take approximately 0.01 ms.
The time suck is therefore either occuring in the function `dgs.receiveFrom` or somewhere in the networking or networking layer.
We have reason to believe that unaccounted time is not occuring in the communication/networking because we are running these tests over localhost, which the operating system speeds up substantially.

Based on this information, we felt that it was unwise to further improve our cache code, as our print statements suggest that it is not a major bottleneck; in fact, they suggest the opposite: the cache is quite fast.
The print statements say that calling `cache_get` takes rougly 0.005 ms.
I should not that all timings are measuring the performance of the cache on a single request, such that multithreading is not used, and there is no locking.
We have some locks on our cache, so this time will be slower if the UDP server receives multiple requests at once.

## Optimizations before timings
Before we finished our timing script, I implemented some optimizations based on the output of our perf report.
The perf report gave very flat timings - everything was less than 2% - which I now know means that I should have removed `-Og` or something else to make the report more accurate (I was using sudo).
But I didn't do that.

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

