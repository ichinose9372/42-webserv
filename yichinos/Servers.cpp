#include "Servers.hpp"

Servers::Servers() 
{
    std::cout << YELLOW <<"Servers constructor called" << NORMAL <<std::endl;
}

Servers::~Servers() {}

void Servers::setPort(std::string& port)
{
    std::stringstream ss(port);
    size_t port_num;
    ss >> port_num;
    if (port_num > 65535)
        throw std::runtime_error("port number is too big");

    else if (ss.fail())
        throw std::runtime_error("port number is not a number");
    else
        this->port = port_num;
}

void Servers::setHost(std::string host)
{
    this->host = host;
}

void Servers::setSeverNames(const std::string& sever_names)
{
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
            location.setPath(*it);
        }
        else
        if (*it == "root")
        {
            it++;
            location.setRoot(*it);
        }
        else if (*it == "index")
        {
            it++;
            location.setIndex(*it);
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

