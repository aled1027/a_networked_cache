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

## Usage for benchmarking
Currently, we have a python script, `performance_tests.py` which does the brunt of the heavy lifting for our benchmark tests. If you want to run the tests yourself, you need to have a few python libraries installed. The file `requirements.txt` lists them. To install, (ideally) activate a new virtual environment and then `pip install -r requirements.txt`. 

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

## 9. Analyze and interpret data
asdf

## 10. Present results
asdf

## Misc notes
- Added a bit of crap to `poco_server.cpp` to `MyUDPServer::runTask` to allow python sockets to work.
- I couldn't get a udp socket to go to `localhost:port/key`, only to `localhost:port` with a message.
- should be a way to it, but wasn't working for me.




