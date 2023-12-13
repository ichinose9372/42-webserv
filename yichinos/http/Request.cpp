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


void Request::remakeUri(ExclusivePath& exclusivePath, Locations& location, std::string servers_root)
{
    std::string path = exclusivePath.getPath(); //locationのrootかaliasを取得
    if (path.empty())
        path = servers_root;
    else 
        path = getAbsolutepath(path, servers_root);
    std::vector<std::string> indexs = location.getIndex();//locationのindexを取得
    if (indexs.empty())
        indexs.push_back("");
    uri = getAbsolutepath(indexs.front(), path);
}

bool Request::checkRequestmethod(Locations& location)
{
   std::vector<std::string>::const_iterator it = location.getMethod().begin();
    for(; it != location.getMethod().end(); it++)
    {
         if (*it == method)
              return false;
    }
    return true;
}


void Request::remakeRequest(Servers& server)
{
    std::vector<Locations> locations = server.getLocations();
    for(std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); it++) //リクエストに対してのlocationを探す
    {   
        if (uri == it->getPath()) //locationが一致した場合
        {
            if (it->getReturnCode().first != 0)//returnCodeが設定されている場合
            {
                returnParameter = it->getReturnCode();
                return;
            }
            if (it->getAutoindex()) // autoindexが設定されている場合
            {
                uri = getAbsolutepath("autoindex.html", server.getRoot());
                return;
            }
            if (checkRequestmethod(*it)) // locationのmethodとリクエストmethodが一致しない場合
            {
                returnParameter.first = 405;
                returnParameter.second = "405.html";
                return;
            }
            if (it->getMaxBodySize() != 0) // maxBodySizeが設定されている場合
            {
                max_body_size = it->getMaxBodySize();
            }
            ExclusivePath exclusivePath = it->getExclusivePath();
            remakeUri(exclusivePath, *it, server.getRoot());
            break;
        }
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

const std::pair<int, std::string>& Request::getReturnParameter() { return returnParameter; }

size_t Request::getMaxBodySize() { return max_body_size; }

void Request::setMethod(const std::string& method) { this->method = method; }

void Request::setUri(const std::string& uri) { this->uri = uri; }

void Request::setHttpVersion(const std::string& httpVersion) { this->httpVersion = httpVersion; }

void Request::setHeaders(std::string key, std::string value) { headers[key] = value; }

void Request::setHost(const std::string& host) { this->host = host; }

void Request::setBody(const std::string& body) { this->body = body; }


