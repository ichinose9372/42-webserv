#include "ExclusivePath.hpp"
#include <iostream>

void ExclusivePath::setRoot(const std::string& path)
{
    if (pathType == ALIAS || pathType == ROOT)
    {
        std::cout << this->path << std::endl;
        throw std::runtime_error("Parse error: Duplicate root");
    }
    this->path = path;
    this->pathType = ROOT;
}

void ExclusivePath::setAlias(const std::string& path)
{
    if (pathType == ALIAS || pathType == ROOT)
        throw std::runtime_error("Parse error: Duplicate alias");
    this->path = path;
    this->pathType = ALIAS;
}

const std::string& ExclusivePath::getPath(void)
{
    if (pathType == NONE)
        return (this->path);
    return (this->path);
}

void ExclusivePath::setPathType(PathType pathType)
{
    this->pathType = pathType;
}