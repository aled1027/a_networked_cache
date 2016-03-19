# http://faisalzqureshi.blogspot.com/2011/02/boost-with-cmake.html
# https://github.com/eidheim/Simple-Web-Server/blob/master/CMakeLists.txt
cmake_minimum_required (VERSION 0.0)
project (a_networked_cache)


#find_package(Boost 1.54.0 COMPONENTS system REQUIRED)
find_package(Boost 1.54.0 COMPONENTS regex system thread coroutine context filesystem date_time REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_compile_options(-std=c++11)
#add_compile_options(-pthread)

add_executable(a_networked_cache src/main.cpp)
target_link_libraries(a_networked_cache
    ${Boost_LIBRARIES})

