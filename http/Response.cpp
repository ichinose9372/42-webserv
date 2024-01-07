#include "Response.hpp"

Response::Response() {}

Response::~Response() {}

// ステータスコードとステータスメッセージのマップを定義
const std::map<int, std::string> STATUS_CODES = {
    {200, "200 OK"},
    {403, "403 Forbidden"},
    {404, "404 Not Found"},
    {500, "500 Internal Server Error"}
    // 他のステータスコードも必要に応じて追加
};

const std::string getStatusMessage(int statusCode)
{
    std::map<int, std::string>::const_iterator it = STATUS_CODES.find(statusCode);
    if (it != STATUS_CODES.end())
        return it->second;
    else
        return STATUS_CODES.find(500)->second;
}

void Response::setResponse()
{
    response = "HTTP/1.1 ";
    response += status;
    response += "\r\n";
    response += allgetHeader();
    response += "\r\n";
    response += body;
    // std::cout << response << std::endl;
}

void Response::setStatus(const std::string &status)
{
    this->status = status;
}

std::string Response::allgetHeader()
{
    std::string header;
    std::map<std::string, std::string>::iterator it;
    for (it = headers.begin(); it != headers.end(); it++)
    {
        header += it->first;
        header += it->second;
        header += "\r\n";
    }
    return header;
}

void Response::setBody(const std::string &body)
{
    this->body = body;
}

void Response::setHeaders(const std::string &key, const std::string &value)
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