#include "Locations.hpp"

Locations::Locations()
{
    path = "/";
    autoindex = false;
    exclusivePath.setPathType(ExclusivePath::NONE);
}

Locations::~Locations() {}

void Locations::setPath(const std::string &path)
{
    // std::cout << path << std::endl;
    this->path = path;
}

const std::string &Locations::getPath(void)
{
    return (this->path);
}

void Locations::setIndex(const std::string &index)
{
    // std::cout << index << std::endl;
    this->indexes.push_back(index);
}

const std::vector<std::string> &Locations::getIndex(void)
{
    return (this->indexes);
}

void Locations::setAutoindex(bool autoindex)
{
    // std::cout << autoindex << std::endl;
    this->autoindex = autoindex;
}

void Locations::setReturnCode(int return_code, const std::string &return_page)
{
    this->return_code.first = return_code;
    this->return_code.second = return_page;
}

void Locations::setCgiExtension(const std::string &cgi_extension)
{
    this->cgi_extension = cgi_extension;
}

bool Locations::getAutoindex(void)
{
    return (this->autoindex);
}

const std::pair<int, std::string> &Locations::getReturnCode(void)
{
    return (this->return_code);
}

const std::string &Locations::getCgiExtension(void)
{
    return (this->cgi_extension);
}

void Locations::setExclusivePath(const std::string &path, std::string pathType)
{
    if (pathType == "root")
        this->exclusivePath.setRoot(path);
    else if (pathType == "alias")
        this->exclusivePath.setAlias(path);
    else
        throw std::runtime_error("Parse error: Invalid path type");
}

void Locations::setMaxBodySize(const std::string &max_body_size)
{
    std::stringstream ss(max_body_size);
    size_t tmp_max_body_size;
    ss >> tmp_max_body_size;
    if (tmp_max_body_size > 1000000000)
        throw std::runtime_error("Parse error: max body size is too big");
    else if (ss.fail() || (ss.peek() != EOF))
        throw std::runtime_error("Parse error: max body size is not a number");
    this->max_body_size = tmp_max_body_size;
}

void Locations::setClientMaxBodySize(const std::string &client_max_body_size)
{
    std::stringstream ss(client_max_body_size);
    size_t tmp_client_max_body_size;
    ss >> tmp_client_max_body_size;
    if (tmp_client_max_body_size > 1000000000)
        throw std::runtime_error("Parse error: client max body size is too big");
    else if (ss.fail() || (ss.peek() != EOF))
        throw std::runtime_error("Parse error: client max body size is not a number");
    this->client_max_body_size = tmp_client_max_body_size;
}

const ExclusivePath &Locations::getExclusivePath(void)
{
    return (this->exclusivePath);
}

void Locations::setMethod(const std::string &method)
{
    this->methods.push_back(method);
}

const std::vector<std::string> &Locations::getMethod(void)
{
    return (this->methods);
}

size_t Locations::getMaxBodySize(void)
{
    return (this->max_body_size);
}

size_t Locations::getClientMaxBodySize(void)
{
    return (this->client_max_body_size);
}