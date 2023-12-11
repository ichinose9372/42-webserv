#include "DeleteRequest.hpp"

DeleteRequest::DeleteRequest()
{
}

DeleteRequest::~DeleteRequest()
{
}

void DeleteRequest::handleDeleteRequest(Request& req, Response& res)
{
    std::map<std::string, std::string> headers = req.getHeaders();
        std::string filename = headers["filename"];
        std::string path = req.getUri() + filename;
        if (std::remove(path.c_str()) == 0)
        {
            res.setStatus("200 OK");
            res.setBody("<html><body><h1>200 OK</h1></body></html>");
        }
        else
        {
            res.setStatus("404 Not Found");
            res.setBody("<html><body><h1>404 Not Found</h1></body></html>");
        }
        res.setResponse();
}