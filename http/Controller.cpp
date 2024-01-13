#include "Controller.hpp"

Controller::Controller()
{
}

Controller::~Controller()
{
}

std::string Controller::getFilepath(Request &req)
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

void Controller::setReturnCode(Request &req, Response &res)
{
    int returnCode = req.getReturnParameter().first;
    std::string responseHtml = getResponseHtml(returnCode);

    res.setStatus(res.getStatusMessage(returnCode));
    res.setHeaders("Content-Type: ", "text/html");
    res.setBody(responseHtml);
    res.setHeaders("Content-Length: ", std::to_string(responseHtml.size()));
    res.setResponse();
}

std::string Controller::getResponseHtml(int statusCode)
{
    switch (statusCode)
    {
    case 301:
        return "<html><body><h1>301 Moved Permanently</h1></body></html>";
    case 403:
        return "<html><body><h1>403 Forbidden</h1></body></html>";
    case 404:
        return "<html><body><h1>404 Not Found</h1></body></html>";
    case 405:
        return "<html><body><h1>405 Method Not Allowed</h1></body></html>";
    case 500:
        return "<html><body><h1>500 Internal Server Error</h1></body></html>";
    // 他のステータスコードに対するHTMLテキスト
    default:
        return "<html><body><h1>Error</h1><p>Unknown Error Occurred</p></body></html>";
    }
}

void Controller::processFile(Request &req, Response &res)
{
    if (req.getReturnParameter().first != 0)
    {
        setReturnCode(req, res);
        return;
    }
    std::string method = req.getMethod();
    if (method == "GET")
        GetRequest::handleGetRequest(req, res);
    else if (method == "POST")
        PostRequest::handlePostRequest(req, res);
    else if (method == "DELETE")
        DeleteRequest::handleDeleteRequest(req, res);
    else
    {
        res.setStatus(res.getStatusMessage(405));
        res.setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
    }
}
