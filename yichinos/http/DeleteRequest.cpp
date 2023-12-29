#include "DeleteRequest.hpp"

DeleteRequest::DeleteRequest()
{
}

DeleteRequest::~DeleteRequest()
{
}

void DeleteRequest::handleDeleteRequest(Request& req, Response& res)
{
    std::string path = Controller::getFilepath(req);
    if (std::remove(path.c_str()) == 0)
    {
        res.setStatus("200 OK");
        res.setBody("<html><body><h1>200 OK</h1></body></html>");
        res.setHeaders("Content-Type", "text/html");
        res.setHeaders("Content-Length", std::to_string(res.getBody().size()));
    }
    else
    {
        res.setStatus("404 Not Found");
        res.setBody("<html><body><h1>404 Not Found</h1></body></html>");
        res.setHeaders("Content-Type", "text/html");
        res.setHeaders("Content-Length", std::to_string(res.getBody().size()));
    }
    res.setResponse();
}