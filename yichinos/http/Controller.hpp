#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <iostream>
#include <string>
#include "Request.hpp"
#include "Response.hpp"
#include <sstream>
#include <fstream>

class Controller
{
    public:
        Controller();
        ~Controller();
        std::string openFile(const std::string& filePath);
        std::string getBody(const std::string& status, const std::string& filePath);
        void processFile(Request& req, Response& res);
    private:       
};

#endif