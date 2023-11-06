#include "Locations.hpp"

Locations::Locations()
{
//     std::cout <<"Locations constructor called" <<std::endl;
}

Locations::~Locations() {}

void Locations::setPath(const std::string& path)
{
    // std::cout << path << std::endl;
    this->path = path;
}

const std::string& Locations::getPath(void)
{
    return (this->path);
}

void Locations::setRoot(const std::string& root)
{
    // std::cout << root << std::endl;
    this->root = root;
}

const std::string& Locations::getRoot(void)
{
    return (this->root);
}

void Locations::setIndex(const std::string& index)
{
    // std::cout << index << std::endl;
    this->locations_indexs.push_back(index);
}

const std::vector<std::string>& Locations::getIndex(void)
{
    return (this->locations_indexs);
}