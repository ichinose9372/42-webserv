#include "Locations.hpp"

Locations::Locations()
{
    std::cout <<"Locations constructor called" <<std::endl;
}

Locations::~Locations() {}

void Locations::setPath(const std::string& path)
{
    this->path = path;
}

const std::string& Locations::getPath(void)
{
    return (this->path);
}

void Locations::setRoot(const std::string& root)
{
    this->root = root;
}

const std::string& Locations::getRoot(void)
{
    return (this->root);
}

void Locations::setIndex(const std::string& index)
{
    this->index = index;
}

const std::string& Locations::getIndex(void)
{
    return (this->index);
}