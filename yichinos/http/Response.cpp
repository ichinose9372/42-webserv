#include "Response.hpp"

Response::Response() {}

Response::~Response() {}

void Response::setResponse()
{
    response = "HTTP/1.1 ";
    response += status;
    response += "\r\n";
    response += allgetHeader();
    response += "\r\n";
    response += body;
}  

void Response::setStatus(const std::string& status)
{
    this->status = status;
}

std::string Response::allgetHeader()
{
    std::string header;
    std::map<std::string, std::string>::iterator it;
    for(it = headers.begin(); it != headers.end(); it++)
    {
        header += it->first;
        header += it->second;
        header += "\r\n";
    }
    return header;
}

void Response::setBody(const std::string& body)
{
    this->body = body;
}

void Response::setHeaders(const std::string& key, const std::string& value)
{
    headers[key] = value;
}

const std::string Response::getBody()
{
    return body;
}

const std::string Response::getResponse()
{
    return response;
}

const std::string Response::getStatus()
{
    return status;
}