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
    extern bool USE_UDP;
    extern bool IS_PYTHON_CLIENT;
    extern bool DEBUG_ON;
    extern bool IS_LOCKING;
}

void print_vector_string(std::vector<std::string> v);

std::vector<std::string> string_split(const std::string &text, char sep);
