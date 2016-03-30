#pragma once
#ifndef poco_client_h
#define poco_client_h

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPClientSession.h>

#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/Exception.h>
#include <Poco/URI.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>  // for copy
#include <iterator>

#include "globals.h"

class Client {
    public:
        Poco::Net::HTTPClientSession client_session;
        Client() : client_session(globals::HOST, globals::PORT) {

            
        }

        void get(const uint8_t* key) {
            try {
                std::ostringstream oss;
                oss << "http://" << globals::HOST << ":" << globals::PORT << "/" << key;
                std::string uri_str = oss.str();

                // Prepare and send request
                Poco::URI uri(uri_str);
                std::string path(uri.getPathAndQuery());
                Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET,
                        path, Poco::Net::HTTPMessage::HTTP_1_1);
                client_session.sendRequest(req);
                std::cout << "sent request" << std::endl;

                // get response
                Poco::Net::HTTPResponse res;
                std::istream& is = client_session.receiveResponse(res);
                std::cout << "received response" << std::endl;

                // TODO
                // do something with response. see docs

                
            } catch (Poco::Exception &e) {
                std::cerr << "Exception: " << e.what() << std::endl;
            }
        }

        ~Client() {}
};

#endif
