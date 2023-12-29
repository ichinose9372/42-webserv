#ifndef EXECCGI_HPP
#define EXECCGI_HPP

#include "Controller.hpp"

class ExecCgi
{
    public:
        ExecCgi();
        ~ExecCgi();
        static void executeCgiScript(Request& req, Response& res);
        static std::string getBody(const std::string& status, const std::string& filePath);
};

#endif