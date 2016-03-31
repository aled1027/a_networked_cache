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
    extern const int PORT;
    extern const char* HOST;
}

void print_vector_string(std::vector<std::string> v);

std::vector<std::string> string_split(const std::string &text, char sep);
