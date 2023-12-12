#include "PostRequest.hpp"

PostRequest::PostRequest() {}

PostRequest::~PostRequest() {}

void PostRequest::handlePostRequest(Request& req, Response& res)
{
    std::string path = Controller::getFilepath(req);
    std::ofstream outputFile(path);
    if (!outputFile.is_open())
    {
        res.setStatus("404 Not Found");
        res.setBody("<html><body><h1>404 Not Found</h1></body></html>");
        res.setResponse();
        return;
    }
    std::string body = req.getBody();
    outputFile << body;
    outputFile.close();
    res.setStatus("200 OK");
    res.setBody("<html><body><h1>200 OK</h1></body></html>");
    res.setHeaders("Content-Type", "text/html");
    res.setHeaders("Content-Length", std::to_string(res.getBody().size()));
    res.setResponse();
}
