#include "MainConfig.hpp"
#include <iostream>
#include <string>
#include <exception>


int main()
{
    try
    {
        MainConfig my("test.cnf");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return (0);
}