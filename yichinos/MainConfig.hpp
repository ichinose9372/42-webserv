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
        std::vector<Servers> servers;
        std::ifstream file;
        void lineToToken(std::string& line);
        void inputServers(std::vector<std::string>::iterator& it, Servers& server);
        MainConfig();
    public:
        void tokenSerch(void);
        void parseLine(void);
        MainConfig(const std::string& filename);
        ~MainConfig();
};

#endif

