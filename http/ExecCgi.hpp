#ifndef EXECCGI_HPP
#define EXECCGI_HPP

#include "Controller.hpp"

class ExecCgi
{
    public:
        ExecCgi();
        ~ExecCgi();
        static void executeCgiScript(Request& req, Response& res);
};

#endif