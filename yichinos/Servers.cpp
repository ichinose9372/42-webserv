#include "Servers.hpp"

Servers::Servers() 
{
    // std::cout << YELLOW <<"Servers constructor called" << NORMAL <<std::endl;
}

Servers::~Servers() {}


// ;　を削除する関数
std::string  removeTrailingSemicolon(std::string& str) 
{
    if (!str.empty() && *str.rbegin() == ';') 
    {
        return (str.erase(str.length() - 1));
    }
    return (str);
}

void Servers::setPort(const std::string& port)
{
    std::stringstream ss(port);
    size_t port_num;
    ss >> port_num;
    if (port_num > 65535)
        throw std::runtime_error("port number is too big");

    else if (ss.fail())
        throw std::runtime_error("port number is not a number");
    else
    {
        std::cout << port_num << std::endl;
        this->port = port_num;
    }
}

void Servers::setHost(const std::string&  host)
{
    std::cout << host << std::endl;
    this->host = host;
}

void Servers::setSeverNames(const std::string& sever_names)
{
    std::cout << sever_names << std::endl;
    this->sever_names.push_back(sever_names);
}

void Servers::setLocations(std::vector<std::string>::iterator& it)
{
    Locations location;
    while (*it != "}")
    {
        if (*it == "path")
        {
            it++;
            location.setPath(removeTrailingSemicolon(*it));
        }
        else
        if (*it == "root")
        {
            it++;
            location.setRoot(removeTrailingSemicolon(*it));
        }
        else if (*it == "index")
        {
            it++;
            location.setIndex(removeTrailingSemicolon(*it));
        }
        it++;
    }
    locations.push_back(location);
}

size_t Servers::getPort(void)
{
    return (this->port);
}

const std::string& Servers::getHost(void)
{
    return (this->host);
}

