#include "MainConfig.hpp"
MainConfig::MainConfig() {}

MainConfig::MainConfig(const std::string& filename) 
{
    file.open(filename.c_str());
    if (!file.is_open())
        throw std::runtime_error("[KO] file not found");
    parseLine();
    parseConfigurationTokens();
    file.close();
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
        // std::cout << line << std::endl;
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

void MainConfig::parseServerBlock(std::vector<std::string>::iterator& it) 
{
    Servers server;
    ++it;
    if (it == tokens.end())
        throw std::runtime_error("Parse error: Unexpected end of file, expecting '{' for server block");
    if (*it != "{")
        throw std::runtime_error("Parse error: Expected '{' after server keyword");
    ++it;
    while (it != tokens.end() && *it != "}") 
    {
        parseServerBlock(it, server);
        ++it;
    }
    if (it == tokens.end() && *it != "}")
        throw std::runtime_error("Parse error: server block not closed with '}'");
    servers.push_back(server);
}

void MainConfig::parseClientMaxBodySize(std::vector<std::string>::iterator& it) 
{
    it++;
    removeTrailingSemicolon(*it);
    setClientMaxBodySize(*it);
}

void MainConfig::parseConfigurationTokens() 
{
    std::vector<std::string>::iterator it = tokens.begin();
    while (it != tokens.end()) 
    {
        if (*it == "server")
            parseServerBlock(it);
        else if (*it == "client_max_body_size")
            parseClientMaxBodySize(it);
        else
            throw std::runtime_error("Parse error: Unexpected token in main block");
        ++it;
    }
}

bool MainConfig::isPortUsed(const size_t& port)
{
    for (std::vector<Servers>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->getPort() == port)
            return true;
    }
    return false;
}

size_t MainConfig::validatePort(const std::string& port )
{
    std::stringstream ss(port);
    size_t port_num;
    ss >> port_num;
    // std::cout << port_num << std::endl;
    if (1024 > port_num ||port_num > 65535)
        throw std::runtime_error("port number is out of range");
    else if (ss.fail() || (ss.peek() != EOF))
        throw std::runtime_error("port number is not a number");
    return port_num;
}

void MainConfig::setClientMaxBodySize(const std::string& client_max_body_size)
{
    if (client_max_body_size.empty()) 
        throw std::runtime_error("Parse error: client_max_body_size is empty");
    if (this->client_max_body_size != 0) 
        throw std::runtime_error("Parse error: Duplicate client_max_body_size");
    size_t pos = 0;
    while (pos < client_max_body_size.size() && std::isdigit(client_max_body_size[pos]))
        pos++;
    std::string numberPart = client_max_body_size.substr(0, pos);
    std::string unitPart = client_max_body_size.substr(pos);
    if (!unitPart.empty() && unitPart.find_first_not_of("kKmMgG") != std::string::npos) 
        throw std::runtime_error("Parse error: Invalid unit for client_max_body_size");
    std::stringstream ss(numberPart);
    int value;
    ss >> value;
    if (ss.fail() || !ss.eof()) 
        throw std::runtime_error("Parse error: Invalid number format for client_max_body_size");
    if (unitPart.empty()) 
        this->client_max_body_size = value;
    else if (unitPart == "k" || unitPart == "K")
        this->client_max_body_size = value * 1024;
    else if (unitPart == "m" || unitPart == "M")
        this->client_max_body_size = value * 1024 * 1024;
    else if (unitPart == "g" || unitPart == "G")
        this->client_max_body_size = value * 1024 * 1024 * 1024;
    else
        throw std::runtime_error("Parse error: Invalid client_max_body_size");
}


void MainConfig::parseServerBlock(std::vector<std::string>::iterator& it, Servers& server)
{
    if (*it == "listen") 
        handleListen(it, server);
    else if (*it == "server_name") 
        handleServerName(it, server);
    else if (*it == "index") 
        handleIndex(it, server);
    else if (*it == "location") 
        handleLocation(it, server);
    else if (*it == "client_max_body_size") 
        handleClientMaxBodySize(it, server);
    else if (*it == "root") 
        handleRoot(it, server);
    else if (*it == "error_page")
        hadleErrorPage(it,server);
    else 
    {
        std::cout << "it: " << *it << std::endl;
        throw std::runtime_error("Parse error: Unexpected token in server block");
    }
}

const size_t& MainConfig::getClientMaxBodySize(void) const
{
    return (this->client_max_body_size);
}


void MainConfig::isServerName(const std::string& server_name, size_t port)
{
    for (std::vector<Servers>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        const std::string names = it->getServerNames();
        size_t tmp_port = it->getPort();
        if (names == server_name && tmp_port == port)
        {
            throw std::runtime_error("Parse error: Duplicate server name");
        }
    }
}

const std::vector<Servers>& MainConfig::getServers(void) const
{
    return (this->servers);
}

void MainConfig::handleListen(std::vector<std::string>::iterator& it, Servers& server) 
{
    it++;
    removeTrailingSemicolon(*it);
    size_t port = validatePort(*it);
    if(isPortUsed(port) )
        isServerName(server.getServerNames(), port);
    server.setPort(port);
}

void MainConfig::handleServerName(std::vector<std::string>::iterator& it, Servers& server) 
{
    it++;
    removeTrailingSemicolon(*it);
    isServerName(*it, server.getPort());
    server.setSeverNames(*it);
}

void MainConfig::handleIndex(std::vector<std::string>::iterator& it, Servers& server) 
{
    it++;
    while (it != tokens.end() && it->find(";") == std::string::npos)
    {
        server.setIndex(*it);
        it++;
    }
    if (it == tokens.end())
        throw std::runtime_error("Parse error: Unexpected end of tokens before index block");
    removeTrailingSemicolon(*it);
    server.setIndex(*it);
}

void MainConfig::handleLocation(std::vector<std::string>::iterator& it, Servers& server) 
{
    it++;
    std::vector<std::string>::iterator end = tokens.end();
    server.setLocations(it, end);
}

void MainConfig::handleClientMaxBodySize(std::vector<std::string>::iterator& it, Servers& server) 
{
    it++;
    removeTrailingSemicolon(*it);
    server.setClientMaxBodySize(*it);
}

void MainConfig::handleRoot(std::vector<std::string>::iterator& it, Servers& server) 
{
    it++;
    removeTrailingSemicolon(*it);
    server.setRoot(*it);
}

void MainConfig::hadleErrorPage(std::vector<std::string>::iterator& it, Servers& server)
{
     while (it != tokens.end()) 
    {
        it++;
        std::string statusCode = *it;
        ++it; // 次のトークンへ移動

        if (it != tokens.end() && it->find(";") != std::string::npos) 
        {
            removeTrailingSemicolon(*it);
            std::string errorPagePath = *it;
            server.setErrorPage(statusCode, errorPagePath);
            return;
        }
        std::string errorPagePath = *it;
        server.setErrorPage(statusCode, errorPagePath);
    }
    
}