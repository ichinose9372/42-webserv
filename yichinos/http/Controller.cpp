#include "Controller.hpp"


Controller::Controller()
{
}

Controller::~Controller()
{
}

std::string Controller::getFilepath(Request& req)
{
    std::map<std::string, std::string> headers = req.getHeaders();
    std::vector<std::string> hedderValueTokens = Request::split(headers["Content-Disposition"], ';');
    std::string filename = hedderValueTokens[2];
    filename = filename.substr(10, filename.size() - 11);
    std::string path = req.getUri() + filename;
    return path;
}

void setReturnCode(Request& req, Response& res)
{
    std::string returnCode = req.getHeaders().find("returnCode")->second;
    res.setStatus(returnCode + "Moved Permanently");
    std::string returnPage =  req.getHeaders().find("returnPage")->second;
    res.setHeaders("Location: ",  returnPage + "\r\n");
    res.setResponse();
}

void Controller::processFile(Request& req, Response& res) 
{
    if (req.getHeaders().find("returnCode") != req.getHeaders().end())
    {
        std::cout << "setReturnCode" << std::endl;
        setReturnCode(req, res);
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
