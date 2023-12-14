#ifndef GETREQUEST_HPP
#define GETREQUEST_HPP

#include "Controller.hpp"
#include "ExecCgi.hpp"

class GetRequest
{
    public:
        GetRequest();
        ~GetRequest();
        static void handleGetRequest(Request& req, Response& res);
        static void executeCgiScript(Request& req, Response& res);
        static std::string openFile(const std::string& filePath);
        static std::string getBody(const std::string& status, const std::string& filePath);
};

#endif