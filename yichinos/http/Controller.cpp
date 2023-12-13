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
    int returnCode = req.getReturnParameter().first;
    std::string returnPage =  req.getReturnParameter().second;
    if (returnCode == 301)
    {
        res.setStatus("301 Moved Permanently");
        res.setHeaders("Location: ", returnPage);
    }
    else if (returnCode == 405)
    {
        res.setStatus("405 Method Not Allowed");
        res.setHeaders("Content-Type", "text/html");
        res.setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
        res.setHeaders("Content-Length", std::to_string(res.getBody().size()));
    }
    res.setResponse();
}

void Controller::processFile(Request& req, Response& res) 
{
    if (req.getReturnParameter().first != 0)
    {
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
