#include "Response.hpp"

Response::Response() {}

Response::~Response() {}

const std::string Response::getResponse(const std::string& status, const std::string& body)
{
    std::string response;
    if (status == "200 OK")
    {
        response = "HTTP/1.1 200 OK\r\n";
    }
    else
    {
        response = "HTTP/1.1 404 Not Found\r\n";
    }
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: ";
    response += std::to_string(body.length());
    response += "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;
    return response;
}