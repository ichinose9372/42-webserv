#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "Response.hpp"

class Request
{
    public:
        ~Request();
        Request(const std::string& request);
        //getter
        const std::string& getMethod();
        const std::string& getUri();
        const std::string& getHttpVersion();
        const std::map<std::string, std::string>& getHeaders();
        const std::map<std::string, std::string>& getQueryParameters();
        const std::string& getBody();
    private:
        Request();
        std::string method;
        std::string uri;
        std::string httpVersion;
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> queryParameters;
        std::string body;
        void parseRequest(const std::string& request);
        void printRequest();
};


#endif
