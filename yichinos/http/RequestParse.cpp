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

std::string getfilepathtoURI(const std::string& uri, Request& request)
{
    (void) request;
    std::string return_uri; 
    std::vector<std::string> uriTokens = split(uri, '/');
    if (uriTokens.size() >= 3) // /directory/filename の場合にこの条件に入る
    {
        return_uri = "/" + uriTokens[1];
        std::string filename;
        for(size_t i = 2; i < uriTokens.size(); i++)
        {
            filename += "/" + uriTokens[i];
        }
        request.setFilepath(filename);
    }
    else
        return_uri = uri;
    return return_uri;
}


void RequestParse::parseRequestLine(const std::string& line, Request& request)
{
    std::vector<std::string> requestLineTokens = Request::split(line, ' ');
    if (requestLineTokens.size() >= 3) 
    {
        request.setMethod(requestLineTokens[0]);
        request.setUri(getfilepathtoURI(requestLineTokens[1], request));
        request.setHttpVersion(requestLineTokens[2]);
    }
}

// location separat filename and path


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