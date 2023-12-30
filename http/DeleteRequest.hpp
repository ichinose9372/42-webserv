#ifndef DELETEREQUEST_HPP
#define DELETEREQUEST_HPP

#include "Controller.hpp"

class DeleteRequest
{
    public:
        DeleteRequest();
        ~DeleteRequest();
        static void handleDeleteRequest(Request& req, Response& res);

};

#endif