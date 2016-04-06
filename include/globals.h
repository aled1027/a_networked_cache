#pragma once

#include <sstream>
#include <istream>
#include <vector>
#include <iostream>
#include <iterator>
#include <assert.h>

#define DEBUG

void debug(std::string s);

namespace globals
{
    extern const int TCP_PORT;
    extern const int UDP_PORT1;
    extern const int UDP_PORT2;
    extern const char* HOST;
    extern bool USE_UDP;
}

void print_vector_string(std::vector<std::string> v);

std::vector<std::string> string_split(const std::string &text, char sep);
