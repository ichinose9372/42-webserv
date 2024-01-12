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
    if (headers.find("Content-Disposition") == headers.end())
    {
        return "";
    }
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
        // std::cout << " IN 405 \n";
        res.setStatus("405 Method Not Allowed");
        res.setHeaders("Content-Type: ", "text/html");
        res.setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
        res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    }
    else if (returnCode == 404)
    {
        // std::cout << " IN 404 \n";
        res.setStatus("404 Not Found");
        res.setHeaders("Content-Type: ", "text/html");
        res.setBody("<html><body><h1>404 Not Found</h1></body></html>");
        res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    }
    else if (returnCode == 413)
    {
        res.setStatus("413 Payload Too Long");
        res.setHeaders("Content-Type: ", "text/html");
        res.setBody("<html><body><h1>413 Payload Too Long</h1></body></html>");
        res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    }
    else if (returnCode == 414)
    {
        res.setStatus("414 URI Too Long");
        res.setHeaders("Content-Type: ", "text/html");
        res.setBody("<html><body><h1>414 URI Too Long</h1></body></html>");
        res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    }
    else if (returnCode == 408)
    {
        res.setStatus("408 Request Timeout");
        res.setHeaders("Content-Type: ", "text/html");
        res.setBody("<html><body><h1>408 Request Timeout</h1></body></html>");
        res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    }
    // else if (returnCode == 500)
    // {
    //     res.setStatus("500 Internal Server Error");
    //     res.setHeaders("Content-Type: ", "text/html");
    //     res.setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
    //     res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    // }
    // else if (returnCode == 501)
    // {
    //     res.setStatus("501 Not Implemented");
    //     res.setHeaders("Content-Type: ", "text/html");
    //     res.setBody("<html><body><h1>501 Not Implemented</h1></body></html>");
    //     res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    // }
    // else if (returnCode == 505)
    // {
    //     res.setStatus("505 HTTP Version Not Supported");
    //     res.setHeaders("Content-Type: ", "text/html");
    //     res.setBody("<html><body><h1>505 HTTP Version Not Supported</h1></body></html>");
    //     res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    // }
    // else if (returnCode == 500)
    // {
    //     res.setStatus("500 Internal Server Error");
    //     res.setHeaders("Content-Type: ", "text/html");
    //     res.setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
    //     res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    // }
    // else if (returnCode == 413)
    // {
    //     res.setStatus("413 Request Entity Too Large");
    //     res.setHeaders("Content-Type: ", "text/html");
    //     res.setBody("<html><body><h1>413 Request Entity Too Large</h1></body></html>");
    //     res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    // }
    // else if (returnCode == 500)
    // {
    //     res.setStatus("500 Internal Server Error");
    //     res.setHeaders("Content-Type: ", "text/html");
    //     res.setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
    //     res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    // }
    res.setResponse();
    // std::cout << "res.getResponse() = " << res.getResponse() << std::endl;
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
        // std::cout << "In Controller::processFile" << std::endl;
        GetRequest::handleGetRequest(req, res);
    }
    else if (method == "POST")
    {   
        // std::cout << "In Controller::processFile POST" << std::endl;
        PostRequest::handlePostRequest(req, res);
    }
    else if(method == "DELETE")
    {
        DeleteRequest::handleDeleteRequest(req, res);
    }
}
