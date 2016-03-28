#pragma once

//#define DEBUG
#include <sstream>
#include <istream>
#include <vector>
#include <iostream>
#include <iterator>
#include <assert.h>

// #define DEBUG

void debug(std::string s) {
#ifdef DEBUG
    std::cout << s << std::endl;
#endif
}



namespace globals
{
    const int HELLO_PORT = 50013;
    const char* HELLO_PORT_STR = "50013";
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

enum HTTPMethod {GET, POST, PUT, DELETE, HEAD, ERR};

HTTPMethod string_to_HTTPMethod(std::string s) {
    if (s.compare("GET") == 0) { 
        return GET;
    } else if (s.compare("POST") == 0) {
        return POST;
    } else if (s.compare("PUT") == 0) {
        return PUT;
    } else if (s.compare("DELETE") == 0) {
        return DELETE;
    } else if (s.compare("HEAD") == 0) {
        return HEAD;
    } else {
        return ERR;
    }
}

struct http_info {
    std::string method;
    std::string path;
    std::string version;
    std::string response_status_code;
    std::string reason_phrase;
    std::string body;
};

void print_http(http_info hi) {
    std::cout << "method: " << hi.method << std::endl;
    std::cout << "path: " << hi.path << std::endl;
    std::cout << "version: " << hi.version << std::endl;
    std::cout << "response_status_code: " << hi.response_status_code << std::endl;
    std::cout << "reason_phrase: " << hi.reason_phrase << std::endl;
    std::cout << "body: " << hi.body << std::endl;
}

http_info parse_request(std::string message) {
    // parse message into a struct http_info 

    // first part parses message into vector
    // equivalent to "tokens = message.split()" in python
    std::istringstream iss(message);
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(tokens));
    assert(tokens.size() == 3);

    // put info into http_info struct object
    http_info hi;
    hi.method = tokens[0];
    hi.path = tokens[1];
    hi.version = tokens[2];
    return hi;
}

http_info parse_response(std::string message) {
    // split string by "\n" and " "
    std::istringstream iss(message);
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(tokens));

    //assert(tokens.size() == 3 || tokens.size() == 4);
    //

    http_info hi;
    hi.version = tokens[0];
    hi.response_status_code = tokens[1];
    hi.reason_phrase = tokens[2];

    // reason_phrase
    bool has_two_word_response_phrase = false;
    if (hi.response_status_code.compare("400") == 0) {
        hi.reason_phrase.append(" ");
        hi.reason_phrase.append(tokens[3]);
        has_two_word_response_phrase = true;
    }

    // get body
    uint32_t start = (has_two_word_response_phrase) ? 4 : 3;
    for (uint32_t i = start; i < tokens.size(); ++i) {
        hi.body.append(tokens[i]);
        hi.body.append(" ");
    }

    return hi;
}

