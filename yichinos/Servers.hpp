#ifndef SERVERS_HPP
#define SERVERS_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Locations.hpp"
#include <sstream>
#include <unistd.h>

#define YELLOW "\033[0;33m"
#define NORMAL "\033[0m"

class Locations;

class Servers
{
    private:
        size_t port;
        std::vector<std::string> indexs;
        std::string sever_name;
        bool serverNameset;
        std::vector<Locations> locations;
        std::map<int, std::string> error_pages;
    public:
        Servers();
        ~Servers();
        void setPort(const size_t& port);
        void setIndex(const std::string& index);
        void setSeverNames(const std::string& sever_name);
        void setLocations(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end);
        size_t getPort(void) const;
        const std::string& getHost(void) const;
        const std::string& getServerNames(void) const;
        const std::vector<std::string>& getIndexs(void) const;
        void  checkPathName(const std::string& path);
};

void  removeTrailingSemicolon(std::string& str);
void  checkFileExists(const std::string& filename);
void  checkFileAccess(const std::string& filename);


#endif