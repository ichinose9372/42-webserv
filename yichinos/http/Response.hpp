#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Request.hpp"
#include <sstream>

class Request;

class Response
{
    public:
        ~Response();
        Response();
        const std::string getResponse(const std::string& status, const std::string& body);
    private:
        std::string response;
};

#endif