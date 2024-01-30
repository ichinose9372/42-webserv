#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <iostream>
#include <string>
#include "Request.hpp"
#include "Response.hpp"
#include <sstream>
#include <fstream>
#include "GetRequest.hpp"
#include "PostRequest.hpp"
#include "DeleteRequest.hpp"

class Controller
{
public:
    Controller();
    ~Controller();
    void processFile(Request &req, Response &res);
    void setReturnCode(Request &req, Response &res);
    std::string getResponseHtml(int statusCode, Request req);
    static std::string getFilepath(Request &req);
    static std::string sanitizeFilename(const std::string &filename);
    static std::string fileRead(const int statuscode);
};

#endif