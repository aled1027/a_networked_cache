#pragma once

#include <sstream>
#include <istream>
#include <vector>
#include <iostream>
#include <iterator>
#include <assert.h>

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
    extern const bool IS_LOCKING;
    extern const uint32_t DEFAULT_MAXMEM;
}

void print_vector_string(std::vector<std::string> v);

std::vector<std::string> string_split(const std::string &text, char sep);
