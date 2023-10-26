#include "MainConfig.hpp"

MainConfig::MainConfig() {}

MainConfig::~MainConfig() {}

MainConfig::MainConfig(const std::string& Filepath)
{
    std::ifstream ifs(Filepath);
    if (!ifs)
    {
        throw std::runtime_error("Error: File not found");
    }
    // readfile
}