#include "Servers.hpp"

Servers::Servers() 
{
    // std::cout << YELLOW <<"Servers constructor called" << NORMAL <<std::endl;
}

Servers::~Servers() {}


// ;　を削除する関数
void  removeTrailingSemicolon(std::string& str) 
{
    if (str.empty()) 
    {
        throw std::runtime_error("Parse error: Expected semicolon, but string is empty");
    }
    if (str.back() != ';') 
    {
        throw std::runtime_error("Parse error: Expected semicolon at the end of the string");
    }
    str.pop_back();
    if (!str.empty() && str.back() == ';') {
        throw std::runtime_error("Parse error: Unexpected semicolon");
    }
}

void Servers::setPort(const size_t& port)
{
    this->port = port;
}


void Servers::setIndex(const std::string& index)
{
    // std::cout << index << std::endl;
    this->indexs.push_back(index);
}

void Servers::setSeverNames(const std::string& sever_names)
{
    // std::cout << sever_names << std::endl;
    this->sever_names.push_back(sever_names);
}

void Servers::setLocations(std::vector<std::string>::iterator& it ,std::vector<std::string>::iterator& end) // s
{
     if (it == end) {
        throw std::runtime_error("Parse error: Unexpected end of tokens before location block");
    }

    Locations location;
    // std::cout << "in location block first token is \"  " << *it << "  \" "<<std::endl;
    location.setPath(*it);
    while (it != end && *it != "}") 
    {
        
        if (*it == "path")
        {
            if (++it == end) break;  // Endをチェック
            removeTrailingSemicolon(*it);
            location.setPath(*it);
        }
        else if (*it == "root") 
        {
            if (++it == end) break;  // Endをチェック
            removeTrailingSemicolon(*it);
            location.setRoot(*it);
        } else if (*it == "index") 
        {
            it++;
            while (it != end && it->find(";") == std::string::npos) 
            {
                std::cout << "locations first = " << *it << std::endl;
                location.setIndex(*it);
                it++;
            }
            if (it == end) 
            {
                throw std::runtime_error("Parse error: Unexpected end of tokens before index block");
            }
            removeTrailingSemicolon(*it);
            std::cout << "locations second = " << *it << std::endl;
            location.setIndex(*it);
        }
        if (++it == end) 
        {  
            throw std::runtime_error("Parse error: Location block not closed with '}'");
        }
    }
    if (it != end && *it == "}") 
    {
        it++;  // 次のトークンに進む
    }
    locations.push_back(location);
}

size_t Servers::getPort(void) const
{
    return (this->port);
}

const std::vector<std::string>& Servers::getIndexs(void) const
{
    return (this->indexs);
}

const std::vector<std::string>& Servers::getServerNames(void) const
{
    return (this->sever_names);
}
