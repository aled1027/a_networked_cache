# http://faisalzqureshi.blogspot.com/2011/02/boost-with-cmake.html
# https://github.com/eidheim/Simple-Web-Server/blob/master/CMakeLists.txt
cmake_minimum_required (VERSION 0.0)
project (a_networked_cache)


#find_package(Boost 1.54.0 COMPONENTS system REQUIRED)
find_package(Boost 1.54.0 COMPONENTS regex system thread coroutine context filesystem date_time REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/include")



add_compile_options(-std=c++14)
add_compile_options(-fpermissive)

add_executable(a_networked_cache "src/main.cpp" "src/cache_tests.cpp" "src/cache.cpp" 
    "src/dbLL.cpp" "src/dbLL_tests.cpp" "src/evict_tests.cpp" "src/evict.cpp" "src/node.cpp")

target_link_libraries(a_networked_cache ${Boost_LIBRARIES})
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



