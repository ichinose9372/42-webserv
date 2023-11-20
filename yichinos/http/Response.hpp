#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Request.hpp"
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

class Request;

class Response
{
    public:
        ~Response();
        Response();
        const std::string getResponse();
        const std::string getStatus();
        const std::string getBody();
        void setBody(const std::string& body);
        void setResponse();
        void setStatus(const std::string& status);

    private:
        std::string status;
        std::string body;
        std::string response;
};

#endif