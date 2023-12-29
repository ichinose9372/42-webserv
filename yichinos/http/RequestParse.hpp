#ifndef REQUESTPARSE_HPP
#define REQUESTPARSE_HPP

#include "Request.hpp"

class Request;

class RequestParse
{
    public:
        RequestParse();
        ~RequestParse();
        void parseRequest(Request& request, const std::string& rawRequest);
        void parseRequestLine(const std::string& line, Request& request);
        void parseHeader(const std::string& line, Request& request);
        void parseBody(std::istringstream& requestStream, Request& request);
    private:
        size_t contentLength;
};

#endif