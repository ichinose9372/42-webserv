#include "Servers.hpp"

Servers::Servers() 
{
    // std::cout << YELLOW <<"Servers constructor called" << NORMAL <<std::endl;
    serverNameset = false; 
    portset = false;
    sever_name = "localhost";
    port = 80;
    indexs.push_back("index.html");
    client_max_body_size = 0;
    root = "./";
}

Servers::~Servers() {}



void printLocations(Locations& Location)
{
    std::cout << "path = " << Location.getPath() << std::endl;
    std::cout << "index = ";
    std::vector<std::string> index = Location.getIndex();
    std::vector<std::string>::iterator it = index.begin();
    for (; it != index.end(); it++)
        std::cout << *it << " ";
    std::cout << std::endl;
    std::cout << "autoindex = " << Location.getAutoindex() << std::endl;
    std::cout << "error_pages = ";
    std::map<int, std::string> error_pages = Location.getErrorPages();
    std::map<int, std::string>::iterator it2 = error_pages.begin();
    for (; it2 != error_pages.end(); it2++)
        std::cout << it2->first << " " << it2->second << " ";
    std::cout << std::endl;
    std::cout << "return_code = " << Location.getReturnCode().first << " " << Location.getReturnCode().second << std::endl;
    std::cout << "cgi_extension = " << Location.getCgiExtension() << std::endl;
    std::cout << "method = ";
    std::vector<std::string> method = Location.getMethod();
    std::vector<std::string>::iterator it3 = method.begin();
    for (; it3 != method.end(); it3++)
        std::cout << *it3 << " ";
    std::cout << std::endl;
}


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


void Servers::setPort(const size_t& port)
{
    if (portset)
        throw std::runtime_error("Parse error: Duplicate port");
    this->port = port;
    portset = true;
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
            location.setExclusivePath(*it, "root");
        }
        else if (*it == "alias")
        {
            it++;
            removeTrailingSemicolon(*it);
            // std::cout << "alias path is " << *it << std::endl;
            location.setExclusivePath(*it, "alias");
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
            std::stringstream ss(*it);
            int tmp_return_code;
            ss >> tmp_return_code;
            it++;
            removeTrailingSemicolon(*it);
            location.setReturnCode(tmp_return_code, *it);
        }
        else if (*it == "cgi_path")
        {
            it++;
            removeTrailingSemicolon(*it);
            //if not dot then throw error
            // if (it->find('.') == std::string::npos)
            //     throw std::runtime_error("Parse error: Invalid cgi_extension");
            location.setCgiExtension(*it);
        }
        else if (*it == "upload_path")
        {
            it++;
            removeTrailingSemicolon(*it);
            location.setUploadPath(*it);
        }
        else if (*it == "client_max_body_size")
        {
            it++;
            removeTrailingSemicolon(*it);
            setClientMaxBodySize(*it);
        }
        else if (*it == "method")
        {
            it++;
            while (it != end && it->find(";") == std::string::npos) 
            {
                location.setMethod(*it);
                it++;
            }
            if (it == end) 
                throw std::runtime_error("Parse error: Unexpected end of tokens before method block");
            removeTrailingSemicolon(*it);
            location.setMethod(*it);
        }
        else
        if (++it == end) 
        {  
            throw std::runtime_error("Parse error: Location block not closed with '}'");
        }

    }
    // printLocations(location);
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

size_t Servers::getClientMaxBodySize(void) const
{
    return (this->client_max_body_size);
}

const std::string& Servers::getHost(void) const
{
    return (this->sever_name);
}


const std::string& Servers::getRoot(void) const
{
    return (this->root);
}

void Servers::setRoot(const std::string& root)
{
    this->root = root;
}

