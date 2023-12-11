#include "RequestParse.hpp"

RequestParse::RequestParse() {}

RequestParse::~RequestParse() {}

std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) 
    {
        tokens.push_back(token);
    }
    return tokens;
}


void RequestParse::parseRequest(Request& request, const std::string& rawRequest)
{
    std::istringstream requestStream(rawRequest);
    std::string line;

    std::getline(requestStream, line);
    parseRequestLine(line, request);
    while (std::getline(requestStream, line) && line != "\r") 
         parseHeader(line, request);
    parseBody(requestStream, request);
}


void RequestParse::parseRequestLine(const std::string& line, Request& request)
{
    std::vector<std::string> requestLineTokens = Request::split(line, ' ');
    if (requestLineTokens.size() >= 3) 
    {
        request.setMethod(requestLineTokens[0]);
        request.setUri(requestLineTokens[1]);
        request.setHttpVersion(requestLineTokens[2]);
    }
}

void RequestParse::parseHeader(const std::string& line, Request& request)
{
    std::vector<std::string> headerTokens = split(line, ':');
    if (headerTokens.size() >= 2) 
    {
        std::string key = headerTokens[0];
        std::string value = headerTokens[1];
        request.setHeaders(key, value);
        if (key == "Host") 
            request.setHost(value);
        if (key == "Content-Length") 
            contentLength = std::stoi(value); // don't use stoi
    }
}

void RequestParse::parseBody(std::istringstream& requestStream, Request& request)
{
    if (contentLength > 0) 
    {
        std::string body;
        body.resize(this->contentLength);
        requestStream.read(&body[0], this->contentLength);
        request.setBody(body);
    }
}