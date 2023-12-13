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

class Servers;
class Locatinos;

class Request
{
    public:
        ~Request();
        Request(const std::string& request);
        //getter
        const std::string& getMethod();
        const std::string& getUri();
        const std::string& getHttpVersion();
        const std::map<std::string, std::string>& getHeaders();
        // const std::map<std::string, std::string>& getQueryParameters();
        const std::string& getBody();
        const std::string& getHost();
        const std::pair<int, std::string>& getReturnParameter();
        void remakeRequest(Servers& server);
        size_t getMaxBodySize();
        //setter
        void setMethod(const std::string& method);
        void setUri(const std::string& uri);
        void setHttpVersion(const std::string& httpVersion);
        void setHeaders(std::string key, std::string value);
        void setHost(const std::string& host);
        void setBody(const std::string& body);
        static std::vector<std::string> split(const std::string &s, char delimiter);
        void remakeUri(ExclusivePath& exclusivePath, Locations& location, std::string servers_root);
        bool checkRequestmethod(Locations& location);
        

    private:
        Request();
        std::string method;
        std::string uri;
        std::string httpVersion;
        std::map<std::string, std::string> headers;
        std::string body;
        std::string host;
        std::pair<int, std::string> returnParameter;
        std::string fileName;
        size_t max_body_size;
        void parseRequest(const std::string& request);
        // void printRequest();
};


#endif
