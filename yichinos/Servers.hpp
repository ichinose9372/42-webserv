#ifndef SERVERS_HPP
#define SERVERS_HPP

#include <iostream>
#include <string>
#include <vector>
#include "Locations.hpp"
#include <sstream>

#define YELLOW "\033[0;33m"
#define NORMAL "\033[0m"

class Locations;

class Servers
{
    private:
        size_t port;
        std::string host;
        std::vector<std::string> sever_names;
        std::vector<Locations> locations;
    public:
        Servers();
        ~Servers();
        void setPort(const std::string& port);
        void setHost(const std::string& host);
        void setSeverNames(const std::string& sever_names);
        void setLocations(std::vector<std::string>::iterator& it);
        size_t getPort(void);
        const std::string& getHost(void);
};

std::string  removeTrailingSemicolon(std::string& str);


#endif