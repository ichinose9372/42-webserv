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
        void parseServerBlock(std::vector<std::string>::iterator& it, Servers& server);
    public:
        MainConfig();
        void parseConfigurationTokens(void);
        void parseServerBlock(std::vector<std::string>::iterator& it);
        void parseClientMaxBodySize(std::vector<std::string>::iterator& it);
        void parseLine(void);
        bool isPortUsed(const size_t& port);
        void isServerName(const std::string& server_name, size_t port);
        void setClientMaxBodySize(const std::string& client_max_body_size);
        const size_t& getClientMaxBodySize(void) const;
        const std::vector<Servers>& getServers(void) const;
        size_t validatePort(const std::string& port);
        void handleListen(std::vector<std::string>::iterator& it, Servers& server);
        void handleServerName(std::vector<std::string>::iterator& it, Servers& server);
        void handleIndex(std::vector<std::string>::iterator& it, Servers& server);
        void handleLocation(std::vector<std::string>::iterator& it, Servers& server);
        void handleRoot(std::vector<std::string>::iterator& it, Servers& server);
        void handleClientMaxBodySize(std::vector<std::string>::iterator& it, Servers& server);
        void hadleErrorPage(std::vector<std::string>::iterator& it, Servers& server);
        MainConfig(const std::string& filename);
        ~MainConfig();
};

#endif

