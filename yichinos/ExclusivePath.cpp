#include "ExclusivePath.hpp"

void ExclusivePath::setRoot(const std::string& path)
{
    if (pathType != NONE)
        throw std::runtime_error("Parse error: Duplicate root");
    this->path = path;
    this->pathType = ROOT;
}

void ExclusivePath::setAlias(const std::string& path)
{
    if (pathType != NONE)
        throw std::runtime_error("Parse error: Duplicate alias");
    this->path = path;
    this->pathType = ALIAS;
}

const std::string& ExclusivePath::getPath(void)
{
    if (pathType == NONE)
        throw std::runtime_error("Parse error: No root or alias");
    return (this->path);
}