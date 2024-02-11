#include "Request.hpp"

Request::Request() : _autoindex(false)
{
}

Request::Request(const std::string &request, int bodySize)
{
    this->content_length = bodySize;
    parseRequest(request);
    this->error_page[404] = "./docs/error_page/404.html";
    this->error_page[500] = "./docs/error_page/500.html";
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

// 絶対パスの作成
std::string getAbsolutepath(const std::string &filePath, std::string rootDir)
{
    std::string absolutePath;
    bool rootEndsWithSlash = !rootDir.empty() && rootDir[rootDir.size() - 1] == '/';
    bool filePathStartsWithSlash = !filePath.empty() && filePath[0] == '/';

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

void Request::parseRequest(const std::string &rawRequest)
{
    RequestParse requestParse;
    requestParse.parseRequest(*this, rawRequest);
}

void Request::remakeUri(ExclusivePath &exclusivePath, Locations &location, std::string servers_root)
{
    struct stat statbuf;
    if (!filepath.empty())
    {
        uri = getAbsolutepath(filepath, servers_root);
    }
    if (stat(uri.c_str(), &statbuf) == 0) // uriが存在する場合
    {
        if (S_ISREG(statbuf.st_mode) && access(uri.c_str(), R_OK) == 0)
            return;
        else if (S_ISDIR(statbuf.st_mode))
        {
            std::string path = exclusivePath.getPath(); // locationのrootかaliasを取得
            if (path.empty())
                path = servers_root;
            std::vector<std::string> indexs = location.getIndex(); // locationのindexを取得
            if (indexs.empty())
                indexs.push_back("");
            if (!filepath.empty())
                uri = getAbsolutepath(indexs.front(), uri);
            else
                uri = getAbsolutepath(indexs.front(), path);
        }
    }
    else // uriが存在しない場合
    {
        std::string path = exclusivePath.getPath(); // locationのrootかaliasを取得
        if (path.empty())
            path = servers_root;
        else
            path = getAbsolutepath(path, servers_root);
        std::vector<std::string> indexs = location.getIndex(); // locationのindexを取得
        if (indexs.empty())
            indexs.push_back("");
        if (!filepath.empty())
            uri = getAbsolutepath(filepath, path);
        else
            uri = getAbsolutepath(indexs.front(), path);
    }
}

bool Request::checkRequestmethod(Locations &location)
{
    std::vector<std::string>::const_iterator it = location.getMethod().begin();
    for (; it != location.getMethod().end(); it++)
    {
        if (*it == method)
            return false;
    }
    return true;
}

bool isMatch(const std::string &uri, Locations &location)
{
    // 完全一致をチェック
    if (uri == location.getPath())
    {
        return true;
    }
    // 前方一致をチェック
    if (uri.find(location.getPath()) == 0)
    {
        // サブディレクトリが正しく一致するかを確認
        if (uri[location.getPath().length()] == '/' || location.getPath()[location.getPath().size() - 1] == '/')
        {
            return true;
        }
    }
    return false;
}

bool Request::checkClientMaxBodySize(size_t size)
{
    if (getContentLength() > size)
    {
        returnParameter.first = 413;
        returnParameter.second = "404.html";
        return false;
    }
    return true;
}

void Request::remakeRequest(Servers &server)
{
    std::string tmp;
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        if (it->first == "Host")
            tmp = it->second;
    }
    std::vector<Locations> locations = server.getLocations();
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); it++) // リクエストに対してのlocationを探す
    {
        error_page = server.getErrorpage();
        if (uri == it->getPath()) // locationが一致した場合
        {
            if (it->getReturnCode().first != 0) // returnCodeが設定されている場合
            {
                returnParameter = it->getReturnCode();
                return;
            }
            if (it->getAutoindex()) // autoindexが設定されている場合
            {
                _autoindex = true;
                std::string oldRoot = server.getRoot();
                std::string eracePath = "/";
                oldRoot.erase(oldRoot.size() - eracePath.size());
                std::string newRoot = oldRoot + it->getPath();
                server.setRoot(newRoot);
                if (it->getIndex().front().empty())
                    uri = server.getRoot();
                else
                    uri = getAbsolutepath(it->getIndex().front(), server.getRoot());
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
            if (it->getClientMaxBodySize() != 0) // clientmaxBodySizeが設定されている場合
            {
                client_max_body_size = it->getClientMaxBodySize();
                if (!checkClientMaxBodySize(client_max_body_size))
                    return;
            }
            ExclusivePath exclusivePath = it->getExclusivePath();
            remakeUri(exclusivePath, *it, server.getRoot()); // filepathが設定されているのならURIをfilepathを使って作り直す
            return;
        }
    }
    // locationがない場合前方一致絵尾さがす処理
    std::vector<Locations> locations2 = server.getLocations();
    for (std::vector<Locations>::iterator it2 = locations2.begin(); it2 != locations2.end(); it2++)
        if (isMatch(uri, *it2)) // リクエストに対してのlocationを探す
        {
            error_page = server.getErrorpage();
            if (it2->getReturnCode().first != 0) // returnCodeが設定されている場合
            {
                returnParameter = it2->getReturnCode();
                return;
            }
            if (it2->getAutoindex()) // autoindexが設定されている場合
            {
                _autoindex = true;
                uri = getAbsolutepath("autoindex/app.py", server.getRoot());
                return;
            }
            if (checkRequestmethod(*it2)) // locationのmethodとリクエストmethodが一致しない場合
            {
                returnParameter.first = 405;
                returnParameter.second = "405.html";
                return;
            }
            if (it2->getMaxBodySize() != 0) // maxBodySizeが設定されている場合
            {
                max_body_size = it2->getMaxBodySize();
            }
            if (it2->getClientMaxBodySize() != 0) // clientmaxBodySizeが設定されている場合
            {
                client_max_body_size = it2->getClientMaxBodySize();
                if (!checkClientMaxBodySize(client_max_body_size))
                    return;
            }
            ExclusivePath exclusivePath = it2->getExclusivePath();
            remakeUri(exclusivePath, *it2, server.getRoot()); // filepathが設定されているのならURIをfilepathを使って作り直す
            return;
        }
    // locationがない場合
    if (this->returnParameter.first == 0) // returnCodeが設定されていない場合
    {
        returnParameter.first = 404;
        returnParameter.second = "404.html";
    }
    else
        return;
}

void Request::printRequest()
{
    std::cout << "--- Request ---" << std::endl;
    std::cout << "Method: " << method << std::endl;
    std::cout << "URI: " << uri << std::endl;
    std::cout << "HTTP Version: " << httpVersion << std::endl;
    std::cout << "Headers: " << std::endl;
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << "Body: " << body << std::endl;
    std::cout << "--- Request End ---" << std::endl;
}

const std::string &Request::getMethod() { return method; }

const std::string &Request::getUri() { return uri; }

const std::string &Request::getHttpVersion() { return httpVersion; }

const std::map<std::string, std::string> &Request::getHeaders() { return headers; }

const std::string &Request::getBody() { return body; }

const std::string &Request::getHost() { return host; }

const std::string &Request::getPort() { return port; }

const std::pair<int, std::string> &Request::getReturnParameter() { return returnParameter; }

const std::string &Request::getFilepath() { return filepath; }

size_t Request::getMaxBodySize() { return max_body_size; }

size_t Request::getClientMaxBodySize() { return client_max_body_size; }

const std::string &Request::getErrorpage(int statuscode)
{
    return error_page[statuscode];
}

bool Request::getAutoindex(void) { return _autoindex; }

size_t Request::getContentLength() { return content_length; };

void Request::setMethod(const std::string &method) { this->method = method; }

void Request::setUri(const std::string &uri) { this->uri = uri; }

void Request::setHttpVersion(const std::string &httpVersion) { this->httpVersion = httpVersion; }

void Request::setHeaders(std::string key, std::string value) { headers[key] = value; }

void Request::setHost(const std::string &host) { this->host = host; }

void Request::setPort(const std::string &port) { this->port = port; }

void Request::setBody(const std::string &body) { this->body = body; }

void Request::setFilepath(const std::string &filepath) { this->filepath = filepath; }

void Request::setReturnParameter(int status, std::string filename)
{
    returnParameter.first = status;
    returnParameter.second = filename;
}

void Request::setContentLength(size_t contentLength) { this->content_length = contentLength; }

void Request::setErrorPage(std::map<int, std::string> error_pages) { this->error_page = error_pages; }
