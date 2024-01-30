#ifndef POSTREQUEST_HPP
#define POSTREQUEST_HPP

#include "Controller.hpp"
#include "ExecCgi.hpp"
#include <fstream>
#include <filesystem>

class PostRequest
{
    public:
        PostRequest();
        ~PostRequest();
        static void handlePostRequest(Request& req, Response& res);
};


#endif