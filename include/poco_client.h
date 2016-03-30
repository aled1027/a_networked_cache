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

        Client() {}
        ~Client() {}

        std::string send(std::string& uri_str, const std::string& method, Poco::Net::HTTPResponse& res) {
            // processes request with uri_str and method. Return response in res
            // with body in body
            try {
                Poco::Net::HTTPClientSession client_session(globals::HOST, globals::PORT);
                Poco::URI uri(uri_str);
                std::string path(uri.getPathAndQuery());

                Poco::Net::HTTPRequest req(method, path, Poco::Net::HTTPMessage::HTTP_1_1);
                client_session.sendRequest(req);

                // get response
                std::istream& istream_body = client_session.receiveResponse(res);
                std::stringstream ss_body;
                std::copy(std::istream_iterator<char>(istream_body),
                        std::istream_iterator<char>(),
                        std::ostream_iterator<char>(ss_body));
                std::string body = ss_body.str();
                return body;
            } catch (Poco::Exception &e) {
                std::cerr << "Exception: " << e.what() << std::endl;
            }
        }

        void get(const uint8_t* key) {
            std::ostringstream oss;
            oss << "http://" << globals::HOST << ":" << globals::PORT << "/" << key;
            std::string uri_str = oss.str();

            // res will contain header information 
            // body will contain the body of the response. (white spaces are removed)
            Poco::Net::HTTPResponse res; 
            std::string body = send(uri_str, Poco::Net::HTTPRequest::HTTP_GET, res);

            switch (res.getStatus()) {
                case Poco::Net::HTTPResponse::HTTP_OK:
                    std::cout << "got ok" << std::endl;
                    break;
                default:
                    std::cout << "got not-ok" << std::endl;
                    break;
            }
        }
};

#endif
