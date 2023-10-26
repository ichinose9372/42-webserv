#ifndef MAINCONFIG_HPP
#define MAINCONFIG_HPP

#include <vector>
#include <map>
#include <list>
#include <deque>
#include <fstream>
#include <iostream>

class MainConfig
{
    private:
        std::vector<std::string> Tokens_;
        std::vector<int> Port_;
        std::vector<std::string> Server_name_;
        std::vector<std::string> Root_;
        std::vector<std::string> Index_;
    public:
        MainConfig();
        MainConfig(const std::string& Filepath);
        ~MainConfig();
        // void readFile(const std::string& Filepath);
};

#endif

