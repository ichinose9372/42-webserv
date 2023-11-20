#include "Response.hpp"

Response::Response() {}

Response::~Response() {}

void Response::setResponse()
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
    this->response = response;
}  

void Response::setStatus(const std::string& status)
{
    this->status = status;
}

const std::string Response::getStatus()
{
    return status;
}

void Response::setBody(const std::string& body)
{
    this->body = body;
}

const std::string Response::getBody()
{
    return body;
}

const std::string Response::getResponse()
{
    // std::string response;
    // if (status == "200 OK")
    // {
    //     response = "HTTP/1.1 200 OK\r\n";
    // }
    // else
    // {
    //     response = "HTTP/1.1 404 Not Found\r\n";
    // }
    // response += "Content-Type: text/html\r\n";
    // response += "Content-Length: ";
    // response += std::to_string(body.length());
    // response += "\r\n";
    // response += "Connection: close\r\n";
    // response += "\r\n";
    // response += body;
    return response;
}