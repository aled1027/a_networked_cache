#pragma once

//#define DEBUG
#include <sstream>
#include <istream>
#include <vector>
#include <iostream>
#include <iterator>
#include <assert.h>

#define DEBUG

void debug(std::string s) {
#ifdef DEBUG
    std::cout << s << std::endl;
#endif
}

namespace globals
{
    const int PORT = 9090;
    const char* HOST = "localhost";
}

void print_vector_string(std::vector<std::string> v) {
    std::cout << "Printing vector string" << std::endl;
    for (std::vector<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
        std::cout << *i << std::endl;
}

std::vector<std::string> string_split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

