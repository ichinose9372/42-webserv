#include "MainConfig.hpp"
MainConfig::MainConfig() {}

MainConfig::MainConfig(const std::string& filename) 
{
    file.open(filename);
    if (!file.is_open())
        throw std::runtime_error("[KO] file not found");
    else
        std::cout << GREEN << "[OK]  File opene " << RESET << std::endl;
}

MainConfig::~MainConfig() {}

void MainConfig::parseLine(void)
{
    std::string line;
    while (std::getline(file, line))
        lineToToken(line);
}

void MainConfig::lineToToken(std::string& line)
{
    std::stringstream ss(line);
    std::string token;
    while (ss >> token)
        tokens.push_back(token);
}



void MainConfig::tokenSerch(void)
{
    std::vector<std::string>::iterator it = tokens.begin();
    while (it != tokens.end())
    {
        if (*it == "server")
        {
            Servers server;
            it++;
            if (it == tokens.end())
                throw std::runtime_error("server block is not closed"); 
            if (*it != "{")
                throw std::runtime_error("server block is not opened");
            it+;
            while (it != tokens.end() && *it != "}")
            {
                inputServers(it, server);
                it++;
            }
            if (it == tokens.end())
                throw std::runtime_error("server block is not closed");
            servers.push_back(server);
        }
        it++;
    }
}

//error mana


void MainConfig::inputServers(std::vector<std::string>::iterator& it, Servers& server)
{
    if (*it == "listen")
    {
        it++;
        server.setPort(removeTrailingSemicolon(*it));
    }
    else if (*it == "server_name")
    {
        it++;
        server.setSeverNames(removeTrailingSemicolon(*it));
    }
    else if (*it == "location")
    {
        it++;
        if (*it != "{")
            throw std::runtime_error("location block is not opened");
        it++;
        server.setLocations(it);
    }
    else
        return ;
}


