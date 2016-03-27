#pragma once

//#define DEBUG

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
