#ifndef LOCATIONS_HPP
#define LOCATIONS_HPP

#include <iostream>
#include <string>
#include <map>
#include "ExclusivePath.hpp"
#include <sstream>
#include <vector>
#include <cstdio>

class ExclusivePath;

class Locations
{
    private:
        std::string path;
        std::vector<std::string> indexes;
        ExclusivePath exclusivePath; // root or alias
        bool autoindex;
        std::pair<int, std::string> return_code;
        std::string cgi_extension;//cgi_path
        size_t max_body_size;
        size_t client_max_body_size;
        std::vector<std::string> methods;

    public:
        Locations();
        ~Locations();
        //setter
        void setPath(const std::string& path);
        void setIndex(const std::string& index);
        void setAutoindex(bool autoindex);
        void setExclusivePath(const std::string& path, std::string pathType);
        void setReturnCode(int return_code, const std::string& return_page);
        void setCgiExtension(const std::string& cgi_extension);
        void setMaxBodySize(const std::string& max_body_size);
        void setClientMaxBodySize(const std::string& client_max_body_size);
        void setMethod(const std::string& method);
        //getter
        const std::string& getPath(void);
        const std::vector<std::string>& getIndex(void);
        bool  getAutoindex(void);
        const std::pair<int, std::string>& getReturnCode(void);
        const std::string& getCgiExtension(void);
        const ExclusivePath& getExclusivePath(void);
        const std::vector<std::string>& getMethod(void);
        size_t getMaxBodySize(void);
        size_t getClientMaxBodySize(void);
};

#endif