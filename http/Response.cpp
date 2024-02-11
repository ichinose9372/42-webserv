#include "Response.hpp"

Response::Response() {this->CGI_read_fd = -1, this->status = "0";}

Response::~Response() {}

// ステータスコードとステータスメッセージのマップを定義
std::map<int, std::string> STATUS_CODES;

// STATUS_CODESの初期化関数
static void initializeStatusCodes()
{
    STATUS_CODES[200] = "200 OK";
    STATUS_CODES[301] = "301 Moved Permanently";
    STATUS_CODES[400] = "400 Bad Request";
    STATUS_CODES[403] = "403 Forbidden";
    STATUS_CODES[404] = "404 Not Found";
    STATUS_CODES[405] = "405 Method Not Allowed";
    STATUS_CODES[413] = "413 Payload Too Large";
    STATUS_CODES[500] = "500 Internal Server Error";
    // 他のステータスコードも必要に応じて追加
}

const std::string Response::getStatusMessage(int statusCode)
{
    // 最初の呼び出しでSTATUS_CODESを初期化
    static bool initialized = false;
    if (!initialized)
    {
        initializeStatusCodes();
        initialized = true;
    }

    std::map<int, std::string>::const_iterator it = STATUS_CODES.find(statusCode);
    if (it != STATUS_CODES.end())
        return it->second;
    else
        return STATUS_CODES[500];
}

void Response::setResponse()
{
    response = "HTTP/1.1 ";
    response += status;
    response += "\r\n";
    response += allgetHeader();
    response += "\r\n";
    response += body;
    // std::cout << "---- IN setresponse ---\n" <<response << "\n------------------- "<<std::endl;
}

void Response::setResponse(const std::string &response)
{
    this->response = response;
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

void Response::setCGIreadfd(int fd)
{
    CGI_read_fd = fd;
}

int Response::getCGIreadfd()
{
    return CGI_read_fd;
}