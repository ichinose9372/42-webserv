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
    // std::cout << sever_name << std::endl;
    if (serverNameset)
        throw std::runtime_error("Parse error: Duplicate server name");
    this->sever_name = sever_name;
    serverNameset = true;
}

//check same name at Locations
void Servers::isPathDuplicate(const std::string& path)
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

void checkExpectedToken(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end, const std::string& expectedToken, const std::string& errorMessage) {
    if (it == end) 
        throw std::runtime_error("Parse error: Unexpected end of tokens before " + expectedToken);
    if (*it != expectedToken) 
        throw std::runtime_error("Parse error: Expected " + expectedToken + " " + errorMessage);
}



void Servers::setLocations(std::vector<std::string>::iterator& it ,std::vector<std::string>::iterator& end) // s
{
    if (it == end) {
        throw std::runtime_error("Parse error: Unexpected end of tokens before location block");
    }
    Locations location;
    isPathDuplicate(*it);
    location.setPath(*it++);
    checkExpectedToken(it, end, "{","Expected '{' after location path");

    static bool setErrorPage = true;
    std::string tmp[] =  {
        "root", "alias", "index", "autoindex", "error_page", "return", "cgi_path", "upload_path", "client_max_body_size", "method"
    };
    static const std::set<std::string> validDirectives = std::set<std::string>(tmp, tmp + sizeof(tmp) / sizeof(tmp[0]));

    for (++it; it != end && *it != "}"; ++it) 
    {
        std::cout << *it << std::endl;
        if (validDirectives.find(*it) == validDirectives.end()) // ないものは無視する
            continue;
        if (*it == "error_page" && !setErrorPage) 
            throw std::runtime_error("Parse error: Duplicate error_page");
        if (*it == "autoindex" || *it == "root" || *it == "alias" || *it == "cgi_path" || *it == "upload_path" || *it == "client_max_body_size") 
            processSingleValueDirective(it, end, location, *it);
        else if (*it == "index" || *it == "method" || *it == "return") 
            processMultiValueDirective(it, end, location, *it);
        else if (*it == "error_page") 
            processErrorPageDirective(it, end, location, setErrorPage);
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

void Servers::processSingleValueDirective(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end, Locations& location, const std::string& directive)
{
    if (++it == end) 
        throw std::runtime_error("Parse error: Unexpected end of tokens before " + directive + " value");
    if (directive == "autoindex") 
    {
        removeTrailingSemicolon(*it);
        if (*it == "on")
            location.setAutoindex(true);
        else if (*it == "off")
            location.setAutoindex(false);
        else
            throw std::runtime_error("Parse error: Invalid autoindex value");
    }
    else if (directive == "root") 
    {
        removeTrailingSemicolon(*it);
        location.setExclusivePath(*it, "root");
    }
    else if (directive == "alias") 
    {
        removeTrailingSemicolon(*it);
        location.setExclusivePath(*it, "alias");
    }
    else if (directive == "cgi_path") 
    {
        removeTrailingSemicolon(*it);
        location.setCgiExtension(*it);
    }
    else if (directive == "upload_path") 
    {
        removeTrailingSemicolon(*it);
        location.setUploadPath(*it);
    }
    else if (directive == "client_max_body_size") 
    {
        removeTrailingSemicolon(*it);
        setClientMaxBodySize(*it);
    }
}

void Servers::processMultiValueDirective(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end, Locations& location, const std::string& directive)
{
    if (it == end) 
        throw std::runtime_error("Parse error: Unexpected end of tokens before " + directive + " value");
    if (*it == "index")
    {
        // std::cout << "######" << std::endl;
        it++;
        for (; it != end && it->find(";") == std::string::npos; ++it) 
        {
            location.setIndex(*it);
        }
        if (it == end) 
                throw std::runtime_error("Parse error: Unexpected end of tokens before index block");
        removeTrailingSemicolon(*it); 
        location.setIndex(*it);
    }
    else if (*it == "method") 
    {
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
}

void Servers::processErrorPageDirective(std::vector<std::string>::iterator& it, std::vector<std::string>::iterator& end, Locations& location, bool& setErrorPage)
{
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
        if (tmp_error_code < 400 || tmp_error_code > 599)
            throw std::runtime_error("Parse error: Invalid error code");
        location.setErrorPages(tmp_error_code, *it2);
        it++;
    }
    setErrorPage = false;
}

