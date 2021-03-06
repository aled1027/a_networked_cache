# http://faisalzqureshi.blogspot.com/2011/02/boost-with-cmake.html
# https://github.com/eidheim/Simple-Web-Server/blob/master/CMakeLists.txt
cmake_minimum_required (VERSION 0.0)
project (a_networked_cache C CXX)

### BOOST ###
#find_package(Boost 1.54.0 COMPONENTS regex system thread coroutine context filesystem date_time REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})
#target_link_libraries(a_networked_cache ${Boost_LIBRARIES})

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("/usr/local/include")


SET(GCC_COVERAGE_COMPILE_FLAGS "-fpermissive -std=c++14 -pthread -Og -pg -g")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg -Og -g")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}  -pg -Og -g")

set(THREADS_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET my_app PROPERTY COMPILE_OPTIONS "-pthread" "-lpthread")
  set_property(TARGET my_app PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread" "-lpthread")
endif()

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

### POCO

# Other external libraries
find_library(POCO_FOUNDATION PocoFoundation)
find_library(POCO_NET PocoNet)
find_library(POCO_JSON PocoJSON)
find_library(POCO_UTIL PocoUtil)

message("system: ${CMAKE_SYSTEM_PREFIX_PATH}")
message("poco foundation: ${POCO_FOUNDATION}")
message("poco net: ${POCO_NET}")
message("poco json: ${POCO_JSON}")
message("poco util: ${POCO_UTIL}")

set(EXTERNAL_LIBS
    ${POCO_FOUNDATION}
    ${POCO_NET}
    ${POCO_JSON}
    ${POCO_UTIL}
    ${CMAKE_THREAD_LIBS_INIT}
    )

add_executable(a_networked_cache "src/main.cpp" "src/cache_tests.cpp" "src/cache.cpp" 
    "src/dbLL.cpp" "src/dbLL_tests.cpp" "src/evict_tests.cpp" "src/evict.cpp" "src/node.cpp" "src/globals.cpp" "src/poco_client.cpp" "src/poco_server.cpp" "src/network_tests.cpp" "src/getRealTime.cpp")

target_link_libraries(a_networked_cache 
    ${EXTERNAL_LIBS})


# run commands
add_custom_target(run_client
    COMMAND ./a_networked_cache client
    DEPENDS a_networked_cache
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
    )

add_custom_target(run_server
    COMMAND ./a_networked_cache server
    DEPENDS a_networked_cache
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
    )

# gdb commands
add_custom_target(gdb_client
    COMMAND gdb --args ./a_networked_cache client
    DEPENDS a_networked_cache
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
    )

add_custom_target(gdb_server
    COMMAND gdb --args ./a_networked_cache server
    DEPENDS a_networked_cache
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
    )

add_custom_target(run
    ./a_networked_cache
    DEPENDS a_networked_cache
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
    )

add_custom_target(prof
    COMMAND gprof ./a_networked_cache
    DEPENDS a_networked_cache
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
    )


