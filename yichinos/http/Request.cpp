#include "Request.hpp"

Request::Request(const std::string& request)
{
    parseRequest(request);
    printRequest();
}

Request::~Request()
{
}

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void Request::parseRequest(const std::string& rawRequest) 
{
    std::istringstream requestStream(rawRequest);
    std::string line;

    // リクエストラインの取得と解析
    std::getline(requestStream, line);
    // std::cout << "line = " << line << std::endl;
    std::vector<std::string> requestLineTokens = split(line, ' ');
    if (requestLineTokens.size() >= 3) 
    {
        method = requestLineTokens[0];
        uri = requestLineTokens[1];
        httpVersion = requestLineTokens[2];
    }

    // ヘッダーの解析
   while (std::getline(requestStream, line) && line != "\r" && line != "") 
   {
     std::vector<std::string> headerTokens = split(line, ':');
        if (headerTokens.size() >= 2) 
        {
            std::string headerName = headerTokens[0];
            std::string headerValue = headerTokens[1];
            headers[headerName] = headerValue;
        }
   }
    // ボディの取得（存在する場合）
    std::getline(requestStream, body);
}

void Request::printRequest() 
{
    std::cout << "Method: " << method << std::endl;
    std::cout << "URI: " << uri << std::endl;
    std::cout << "HTTP Version: " << httpVersion << std::endl;
    std::cout << "Headers: " << std::endl;
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) 
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << "Body: " << body << std::endl;
}

const std::string& Request::getMethod() { return method; }

const std::string& Request::getUri() { return uri; }

const std::string& Request::getHttpVersion() { return httpVersion; }
