#ifndef LOCATIONS_HPP
#define LOCATIONS_HPP

#include <iostream>
#include <string>
#include <map>
#include "ExclusivePath.hpp"

class ExclusivePath;

class Locations
{
    private:
        std::string path;
        std::vector<std::string> indexes;
        ExclusivePath exclusivePath; // root or alias
        bool autoindex;
        std::map<int, std::string> error_pages;
        std::pair<int, std::string> return_code;
        std::string cgi_extension;
    public:
        Locations();
        ~Locations();
        //setter
        void setPath(const std::string& path);
        void setIndex(const std::string& index);
        void setAutoindex(bool autoindex);
        void setExclusivePath(const std::string& path, std::string pathType);
        void setErrorPages(int error_code, const std::string& error_page);
        void setReturnCode(int return_code, const std::string& return_page);
        void setCgiExtension(const std::string& cgi_extension);
        //getter
        const std::string& getPath(void);
        const std::vector<std::string>& getIndex(void);
        bool  getAutoindex(void);
        const std::map<int, std::string>& getErrorPages(void);
        const std::pair<int, std::string>& getReturnCode(void);
        const std::string& getCgiExtension(void);
};

#endif