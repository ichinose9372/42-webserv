#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Request.hpp"
#include <sstream>


class Response
{
    public:
        ~Response();
        Response(Request& request);
        Response();
        const std::string& getResponse();
    private:
        std::string response;
        std::string status;
        std::map<std::string, std::string> headers;
        std::string body;
};

#endif