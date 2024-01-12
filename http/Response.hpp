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
    const std::string getStatusMessage(int statusCode);
    const std::string getResponse();
    const std::string getStatus();
    const std::string getBody();
    void setBody(const std::string &body);
    void setResponse();
    void setStatus(const std::string &status);
    void setHeaders(const std::string &key, const std::string &value);
    std::string allgetHeader();

private:
    std::string status;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string response;
};

#endif