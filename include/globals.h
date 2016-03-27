#pragma once

#define DEBUG

void debug(std::string s) 
{
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

struct http_info {
    std::string method;
    std::string path;
    std::string version;
    std::string response_status_code;
    std::string reason_phrase;
};

void print_http(http_info hi) {
    std::cout << "method: " << hi.method << std::endl;
    std::cout << "path: " << hi.path << std::endl;
    std::cout << "version: " << hi.version << std::endl;
    std::cout << "response_status_code: " << hi.response_status_code << std::endl;
    std::cout << "reason_phrase: " << hi.reason_phrase << std::endl;
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
    std::istringstream iss(message);
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(tokens));
    assert(tokens.size() == 3 || tokens.size() == 4);

    http_info hi;
    hi.version = tokens[0];
    hi.response_status_code = tokens[1];
    hi.reason_phrase = tokens[2];
    if (tokens.size() == 4) {
        hi.reason_phrase.append(" ");
        hi.reason_phrase.append(tokens[3]);
    }
    return hi;
}
