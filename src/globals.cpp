#include "globals.h"


void debug(std::string s) {
    std::cout << s << std::endl;
}

namespace globals
{
    const int TCP_PORT = 8080;
    const int UDP_PORT = 8081;
    const char *HOST = "localhost";
    bool USE_UDP = true;
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
