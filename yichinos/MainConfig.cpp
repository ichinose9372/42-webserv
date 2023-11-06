#include "MainConfig.hpp"
MainConfig::MainConfig() {}

MainConfig::MainConfig(const std::string& filename) 
{
    file.open(filename);
    if (!file.is_open())
        throw std::runtime_error("[KO] file not found");
    // else
    //     std::cout << GREEN << "[OK]  File opene " << RESET << std::endl;
}

MainConfig::~MainConfig() {}

void MainConfig::parseLine(void)
{
    std::string line;
    while (std::getline(file, line))
    {
        size_t commentPos = line.find("#");
        if (commentPos != std::string::npos)
        {
            line = line.substr(0, commentPos);
        }
        if (line.empty())
            continue;
        lineToToken(line);
    }
}

void MainConfig::lineToToken(std::string& line)
{
    std::stringstream ss(line);
    std::string token;
    while (ss >> token)
        tokens.push_back(token);
}

void MainConfig::tokenSearch() 
{
    std::vector<std::string>::iterator it = tokens.begin();
    while (it != tokens.end()) 
    {
        if (*it == "server") 
        {
            Servers server;
            ++it;
            if (it == tokens.end())
                throw std::runtime_error("Parse error: Unexpected end of file, expecting '{' for server block");
            
            if (*it != "{")
                throw std::runtime_error("Parse error: Expected '{' after server keyword");
            ++it;
            while (it != tokens.end() && *it != "}") {
                inputServers(it, server);
                ++it;
            }
            
            if (it == tokens.end() && *it != "}")
                throw std::runtime_error("Parse error: server block not closed with '}'");
            servers.push_back(server);
        }
        if (it != tokens.end()) {
            ++it;
        }
    }
}

bool MainConfig::checkPortNum(const size_t& port)
{
    for (std::vector<Servers>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->getPort() == port)
            return true;
    }
    return false;
}

bool MainConfig::checkServerName(const std::string& server_name)
{
    for (std::vector<Servers>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        const std::vector<std::string> names = it->getServerNames();
        for (std::vector<std::string>::const_iterator it2 = names.begin(); it2 != names.end(); ++it2)
        {
            if (*it2 == server_name)
                return true;
        }
    }
    return false;
}

size_t MainConfig::validatePort(const std::string& port)
{
     std::stringstream ss(port);
    size_t port_num;
    ss >> port_num;
    // std::cout << port_num << std::endl;
    if (port_num > 65535)
        throw std::runtime_error("port number is too big");
    else if (ss.fail() || (ss.peek() != EOF))
        throw std::runtime_error("port number is not a number");
    else if (checkPortNum(port_num))
        throw std::runtime_error("port number is not a number");
    return port_num;
}


void MainConfig::inputServers(std::vector<std::string>::iterator& it, Servers& server)
{
    if (*it == "listen")
    {
        it++;
        removeTrailingSemicolon(*it);
        size_t port = validatePort(*it);
        server.setPort(port);
    }
    else if (*it == "server_name")
    {
        it++;
        removeTrailingSemicolon(*it);
        if (checkServerName(*it))
            throw std::runtime_error("server name is not valid");
        server.setSeverNames(*it);
    }
    else if (*it == "index")
    {
        it++;
        while (it != tokens.end() && it->find(";") == std::string::npos)
        {
            checkFileExists(*it);
            checkFileAccess(*it);
            server.setIndex(*it);
            it++;
        }
        if (it == tokens.end())
            throw std::runtime_error("Parse error: Unexpected end of tokens before index block");
        checkFileExists(*it);
        checkFileAccess(*it);
        removeTrailingSemicolon(*it);
        server.setIndex(*it);
    }
    else if (*it == "location")
    {
        it++;
        std::vector<std::string>::iterator end = tokens.end();
        server.setLocations(it, end);
    }
    else
       return ;
}


