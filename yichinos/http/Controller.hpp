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
        void processFile(Request& req, Response& res);
        static std::string getFilepath(Request& req);
        void serRetunrCode(Request& req, Response& res);   
    private:       
};

#endif