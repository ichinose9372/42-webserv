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
    {
        lineToToken(line);
    }
}

void MainConfig::lineToToken(std::string& line)
{
    std::stringstream ss(line);
    std::string token;
    while (ss >> token)
    {
        // std::cout << token << std::endl;
        tokens.push_back(token);
    }
}

void MainConfig::tokenSerch(void)
{
    std::vector<std::string>::iterator it = tokens.begin();
    while (it != tokens.end())
    {
        if (*it == "server")
        {
            //preparation for ther server class
            Servers server;
            it++;
            if (*it == "{")
            {
                it++;
                while (*it != "}")
                {
                    inputServers(it, server);
                    it++;
                }
            }
            servers.push_back(server);
        }
        it++;
    }
}

void MainConfig::inputServers(std::vector<std::string>::iterator& it, Servers& server)
{
    if (*it == "listen")
    {
        it++;
        server.setPort(*it);
        // std::cout << "liten =  " << *it << std::endl;
    }
    else if (*it == "server_name")
    {
        it++;
        server.setSeverNames(*it);
        // std::cout << "server_name =  " << *it << std::endl;
    }
    else if (*it == "location")
    {
        it++;
        server.setLocations(it);
    }
    else
        return ;
}


