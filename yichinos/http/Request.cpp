#include "Request.hpp"

Request::Request(const std::string& request)
{
    parseRequest(request);
    // printRequest();
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


//絶対パスの作成
std::string getAbsolutepath(const std::string& filePath, std::string rootDir) 
{
    std::string absolutePath;
    // std::cout << "filePath = " << filePath << std::endl;
    // std::cout << "rootDir = " << rootDir << std::endl;
    bool rootEndsWithSlash = !rootDir.empty() && rootDir.back() == '/';
    bool filePathStartsWithSlash = !filePath.empty() && filePath.front() == '/';

    if (rootEndsWithSlash && filePathStartsWithSlash) 
        absolutePath = rootDir + filePath.substr(1);
    else if (!rootEndsWithSlash && !filePathStartsWithSlash) 
        absolutePath = rootDir + "/" + filePath;
    else
        absolutePath = rootDir + filePath;

    if (absolutePath[0] != '.') 
        absolutePath = "." + absolutePath;

    return absolutePath;
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
            //search location
        // std::vector<Locations> locations = server.getLocations();
        // for(std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); it++)
        // {
        //     std::string path = it->getPath();
        //     if (requestLineTokens[1] == path)
        //     {
        //         ExclusivePath exclusivePath = it->getExclusivePath();
        //         std::vector<std::string> indexs = it->getIndex();
        //         std::string root = exclusivePath.getPath();
        //         if (root.empty())
        //             root = server.getRoot();
        //         if (indexs.empty())
        //             indexs.push_back("");
        //         uri = getAbsolutepath(indexs.front(), root);
        //         break;
        //     }
        //     else if (requestLineTokens[1].size() == 1 && requestLineTokens[1] == "/")
        //     {
        //         uri = getAbsolutepath("index.html", server.getRoot());
        //         break;
        //     }
        // }
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
            // // if heddername = disposition
            // if (headerName == "Content-Disposition")
            // {
            //     std::vector<std::string> hedderValueTokens = split(headerValue, ';');
            //     if (hedderValueTokens.size() >= 2)
            //     {
            //         std::string filename = hedderValueTokens[2];
            //         filename = filename.substr(10, filename.size() - 11);
            //         headers["filename"] = filename;
            //     }
            // }
            if (headerName == "Host")
            {
                host = headerValue.substr(1, headerValue.size() - 1); //ちゃんとしたtrim関数を作る
            }
        }
   }
    // ボディの取得（存在する場合）
    std::getline(requestStream, body);
}

void Request::remakeRequest(Servers& server)
{
    std::vector<Locations> locations = server.getLocations();
    for(std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); it++)
    {   
        if (uri == it->getPath())
        {
            if (it->getReturnCode().first != 0)
            {
                std::string returnCode = std::to_string(it->getReturnCode().first);
                std::string returnPage = it->getReturnCode().second;
                headers["returnCode"] = returnCode;
                headers["returnPage"] = returnPage;
                return;
            }
            if (it->getAutoindex())
            {
                uri = getAbsolutepath("autoindex.html", server.getRoot());
                return;
            }
            ExclusivePath exclusivePath = it->getExclusivePath();
            std::vector<std::string> indexs = it->getIndex();
            std::string root = exclusivePath.getPath();
            root = getAbsolutepath(root, server.getRoot());
            if (root.empty())
                root = server.getRoot();
            if (indexs.empty())
                indexs.push_back("");
            // uri = getAbsolutepath(indexs.front(), getAbsolutepath(root, server.getRoot()));
            std::cout << "indexs.front() = " << indexs.front() << root << std::endl;
            uri = getAbsolutepath(indexs.front(), root);
            break;
        }
        else if (uri.size() == 1 && uri.front() == '/')
        {
            uri = getAbsolutepath("index.html", server.getRoot());
            break;
        }   
    }
    if (method == "POST" || method == "DELETE")
    {
        std::vector<std::string> hedderValueTokens = split(headers["Content-Disposition"], ';');
        std::string filename = hedderValueTokens[2];
        filename = filename.substr(10, filename.size() - 11);
        headers["filename"] = filename;
    }
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

const std::map<std::string, std::string>& Request::getHeaders() { return headers; }

const std::map<std::string, std::string>& Request::getQueryParameters() { return queryParameters; }

const std::string& Request::getBody() { return body; }

const std::string& Request::getHost() { return host; }

