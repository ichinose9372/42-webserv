#include "Controller.hpp"


Controller::Controller()
{
}

Controller::~Controller()
{
}

void Controller::processFile(Request& req, Response& res) 
{
    if (req.getHeaders().find("returnCode") != req.getHeaders().end())
    {
        std::string returnCode = req.getHeaders().find("returnCode")->second;
        res.setStatus(returnCode);
        std::string returnPage =  req.getHeaders().find("returnPage")->second;
        res.setBody(returnPage);
        res.setResponse();
        return;
    }
    std::string method = req.getMethod();
    if (method == "GET")
    {
      GetRequest::handleGetRequest(req, res);
    }
    else if (method == "POST")
    {   
        PostRequest::handlePostRequest(req, res);
    }
    else if(method == "DELETE")
    {
        DeleteRequest::handleDeleteRequest(req, res);
    }
}
