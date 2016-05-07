#pragma once

#include <sstream>
#include <iostream>
#include <vector>

#define DEBUG


void debug(const std::string& s);

namespace globals
{
    extern const int TCP_PORT;
    extern const int UDP_PORT;
    extern const char* HOST;
    extern const bool USE_UDP;
    extern const bool IS_PYTHON_CLIENT;
    extern const bool DEBUG_ON;
    extern const uint32_t DEFAULT_MAXMEM;
    extern const uint32_t NUM_UDP_THREADS;
}

void print_vector_string(std::vector<std::string> v);

std::vector<std::string> string_split(const std::string &text, char sep);
uint64_t my_get_time();
