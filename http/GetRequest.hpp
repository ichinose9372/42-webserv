#ifndef GETREQUEST_HPP
#define GETREQUEST_HPP

#include "Controller.hpp"
#include "ExecCgi.hpp"
#include <sys/stat.h>

class GetRequest
{
public:
    GetRequest();
    ~GetRequest();
    static void handleGetRequest(Request &req, Response &res);
    static void handleCgiScript(Request &req, Response &res);
    static void handleRegularFile(Request &req, Response &res);
    static int openFile(const std::string &filePath);
    static std::string getBody(const std::string &filePath);
};

#endif