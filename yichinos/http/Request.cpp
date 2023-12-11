#include "Request.hpp"

Request::Request(const std::string& request)
{
    parseRequest(request);
    // printRequest();
}

Request::~Request()
{
}

std::vector<std::string> Request::split(const std::string &s, char delimiter)
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


//絶対パスの作成
std::string getAbsolutepath(const std::string& filePath, std::string rootDir) 
{
    std::string absolutePath;
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
    RequestParse requestParse;
    requestParse.parseRequest(*this, rawRequest);
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

// void Request::printRequest() 
// {
//     std::cout << "Method: " << method << std::endl;
//     std::cout << "URI: " << uri << std::endl;
//     std::cout << "HTTP Version: " << httpVersion << std::endl;
//     std::cout << "Headers: " << std::endl;
//     for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) 
//     {
//         std::cout << it->first << ": " << it->second << std::endl;
//     }
//     std::cout << "Body: " << body << std::endl;
// }

const std::string& Request::getMethod() { return method; }

const std::string& Request::getUri() { return uri; }

const std::string& Request::getHttpVersion() { return httpVersion; }

const std::map<std::string, std::string>& Request::getHeaders() { return headers; }

const std::string& Request::getBody() { return body; }

const std::string& Request::getHost() { return host; }

void Request::setMethod(const std::string& method) { this->method = method; }

void Request::setUri(const std::string& uri) { this->uri = uri; }

void Request::setHttpVersion(const std::string& httpVersion) { this->httpVersion = httpVersion; }

void Request::setHeaders(std::string key, std::string value) { headers[key] = value; }

void Request::setHost(const std::string& host) { this->host = host; }

void Request::setBody(const std::string& body) { this->body = body; }
