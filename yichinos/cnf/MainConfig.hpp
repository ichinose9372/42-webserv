#ifndef MAINCONFIG_HPP
#define MAINCONFIG_HPP

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define RESET "\033[0m"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Servers.hpp"

class Servers;

class MainConfig
{
    private:
        std::vector<std::string> tokens;
        std::ifstream file;
        std::vector<Servers> servers;
        size_t client_max_body_size;
        void lineToToken(std::string& line);
        void inputServers(std::vector<std::string>::iterator& it, Servers& server);
    public:
        MainConfig();
        void tokenSearchandSet(void);
        void parseLine(void);
        bool checkPortNum(const size_t& port);
        void checkServerName(const std::string& server_name, size_t port);
        void setClientMaxBodySize(const std::string& client_max_body_size);
        const size_t& getClientMaxBodySize(void) const;
        const std::vector<Servers>& getServers(void) const;
        size_t validatePort(const std::string& port);
        MainConfig(const std::string& filename);
        ~MainConfig();
};

#endif

