#include "Servers.hpp"

Servers::Servers() 
{
    // std::cout << YELLOW <<"Servers constructor called" << NORMAL <<std::endl;
    serverNameset = false; 
    sever_name = "localhost";
    port = 80;
    indexs.push_back("index.html");
    client_max_body_size = 1000000;
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


void checkFileExists(const std::string& filename)
{
    if (access(filename.c_str(), F_OK) == -1) 
    {
        throw std::runtime_error("Parse error: File does not exist");
    }
}

void checkFileAccess(const std::string& filename)
{
    if (access(filename.c_str(), R_OK) == -1) 
    {
        throw std::runtime_error("Parse error: File is not accessible");
    }
}

void Servers::setClientMaxBodySize(const std::string& client_max_body_size)
{
    if (client_max_body_size.empty()) 
        throw std::runtime_error("Parse error: client_max_body_size is empty");
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


void Servers::setPort(const size_t& port)
{
    if (this->port != 0)
        throw std::runtime_error("Parse error: Duplicate port");
    this->port = port;
}


void Servers::setIndex(const std::string& index)
{
    // std::cout << index << std::endl;
    this->indexs.push_back(index);
}

void Servers::setSeverNames(const std::string& sever_name)
{
    // std::cout << sever_names << std::endl;
    if (serverNameset)
        throw std::runtime_error("Parse error: Duplicate server name");
    this->sever_name = sever_name;
    serverNameset = true;
}

//check same name at Locations
void Servers::checkPathName(const std::string& path)
{
    if (path.empty()) 
    {
        throw std::runtime_error("Parse error: Empty path");
    }
    std::vector<Locations>::iterator it = locations.begin();
    for(; it != locations.end(); it++)
    {
        if (it->getPath() == path)
            throw std::runtime_error("Parse error: Duplicate path");
    }
}

void Servers::setLocations(std::vector<std::string>::iterator& it ,std::vector<std::string>::iterator& end) // s
{
    static bool setErrorPage = true;
    // std::cout << "setLocations called    \"  " << *it << "  \""  <<std::endl;
     if (it == end) {
        throw std::runtime_error("Parse error: Unexpected end of tokens before location block");
    }
    Locations location;
    checkPathName(*it);
    location.setPath(*it);
    it++;
    if (*it != "{")
        throw std::runtime_error("Parse error: Expected '{' after location path");
    while (it != end && *it != "}") 
    {
        if (*it == "root") 
        {
            it++;
            removeTrailingSemicolon(*it);
            // std::cout << "root path is " << *it << std::endl;
            location.setExclusivePath(*it, ExclusivePath::ROOT);
        }
        else if (*it == "alias")
        {
            it++;
            removeTrailingSemicolon(*it);
            // std::cout << "alias path is " << *it << std::endl;
            location.setExclusivePath(*it, ExclusivePath::ALIAS);
        }
        else if (*it == "index") 
        {
            it++;
            while (it != end && it->find(";") == std::string::npos) 
            {
                // checkFileAccess(*it);
                // checkFileExists(*it);
                // std::cout << "index path is " << *it << std::endl;
                location.setIndex(*it);
                it++;
            }
            if (it == end) 
                throw std::runtime_error("Parse error: Unexpected end of tokens before index block");
            // checkFileAccess(*it);
            // checkFileExists(*it);
            removeTrailingSemicolon(*it);
            // std::cout << "index path is " << *it << std::endl;
            location.setIndex(*it);
        }
        else if (*it == "autoindex")
        {
            it++;
            removeTrailingSemicolon(*it);
            if (it == end) 
                throw std::runtime_error("Parse error: Unexpected end of tokens before autoindex value");
            if (*it == "on")
                location.setAutoindex(true);
            else if (*it == "off")
                location.setAutoindex(false);
            else
                throw std::runtime_error("Parse error: Invalid autoindex value");
        }
        else if (*it == "error_page")
        {
            if (setErrorPage == false)
                throw std::runtime_error("Parse error: Duplicate error_page");
            it++;
            std::vector<std::string>::iterator it2 = it;
            while (it2->find(';') == std::string::npos)
            {
                it2++;
                if (it2 == end)
                    throw std::runtime_error("Parse error: Unexpected end of tokens before error_page code");
            }
            removeTrailingSemicolon(*it2);
            while(it != it2)
            {
                std::stringstream ss(*it);
                int tmp_error_code;
                ss >> tmp_error_code;
                // std::cout << "error code is " << tmp_error_code << "  error_page =  " <<*it2 << std::endl;
                if (tmp_error_code < 400 || tmp_error_code > 599)
                    throw std::runtime_error("Parse error: Invalid error code");
                location.setErrorPages(tmp_error_code, *it2);
                it++;
            }
            setErrorPage = false;
        }
        else if (*it == "return")
        {
            it++;
            removeTrailingSemicolon(*it);
            std::stringstream ss(*it);
            int tmp_return_code;
            ss >> tmp_return_code;
            it++;
            // std::cout << "return code is " << tmp_return_code << *it << std::endl;
            location.setReturnCode(tmp_return_code, *it);
        }
        else if (*it == "cgi_extension")
        {
            it++;
            removeTrailingSemicolon(*it);
            //if not dot then throw error
            if (it->find('.') == std::string::npos)
                throw std::runtime_error("Parse error: Invalid cgi_extension");
            location.setCgiExtension(*it);
        }
        if (++it == end) 
        {  
            throw std::runtime_error("Parse error: Location block not closed with '}'");
        }
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

const std::string& Servers::getServerNames(void) const
{
    return (this->sever_name);
}


const std::vector<Locations>& Servers::getLocations(void) const
{
    return (this->locations);
}

const size_t Servers::getClientMaxBodySize(void) const
{
    return (this->client_max_body_size);
}

// void Servers::setdefaultLocations(void)
// {
//     std::vector<Locations>::iterator it = locations.begin();
//     for (; it != locations.end(); it++)
//     {
//         if (it->getPath() == "")
//         {
//             it->setPath("/");
//             it->setAutoindex(true);
//             it->setExclusivePath(".", ExclusivePath::ROOT);
//             it->setIndex("index.html");
//         }
//     }
// }