#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <filesystem>
#include "Response.hpp"
#include "../cnf/Servers.hpp"
#include "../cnf/Locations.hpp"
#include "RequestParse.hpp"
#include <sys/stat.h>
#include "Utils.hpp"

class Servers;
class Locatinos;

class Request
{
public:
    ~Request();
    Request(const std::string &request, int bodySize);
    // getter
    const std::string &getMethod();
    const std::string &getUri();
    const std::string &getFilepath();
    const std::string &getHttpVersion();
    const std::map<std::string, std::string> &getHeaders();
    // const std::map<std::string, std::string>& getQueryParameters();
    const std::string &getBody();
    const std::string &getHost();
    const std::string &getPort();
    const std::pair<int, std::string> &getReturnParameter();
    void remakeRequest(Servers &server);
    size_t getMaxBodySize();
    size_t getClientMaxBodySize();
    size_t getContentLength();
    const std::string &getErrorpage(int statucode);
    bool getAutoindex();
    // setter
    void setMethod(const std::string &method);
    void setUri(const std::string &uri);
    void setFilepath(const std::string &filepath);
    void setHttpVersion(const std::string &httpVersion);
    void setHeaders(std::string key, std::string value);
    void setHost(const std::string &host);
    void setPort(const std::string &port);
    void setBody(const std::string &body);
    void setContentLength(size_t content_length);
    void setReturnParameter(int status, std::string filename);
    void setErrorPage(std::map<int, std::string> error_pages);
    static std::vector<std::string> split(const std::string &s, char delimiter);
    void remakeUri(ExclusivePath &exclusivePath, Locations &location, std::string servers_root);
    bool checkRequestmethod(Locations &location);
    bool checkClientMaxBodySize(size_t size);
    Request();

private:
    std::string method;
    std::string uri;
    std::string filepath;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string host;
    std::string port;
    std::pair<int, std::string> returnParameter;
    std::string fileName;
    size_t max_body_size;
    size_t client_max_body_size;
    size_t content_length;
    std::map<int, std::string> error_page;
    void parseRequest(const std::string &request);
    void printRequest();
    bool _autoindex;
};

#endif
