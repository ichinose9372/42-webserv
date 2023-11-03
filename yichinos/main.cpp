#include "MainConfig.hpp"
#include <iostream>
#include <string>
#include <exception>


int main()
{
    try
    {
        MainConfig myconf("test.cnf");
        myconf.parseLine();//make tokens
        myconf.tokenSerch();//make servers
    }
    catch(const std::exception& e)
    {
        std::cerr << RED << e.what() << '\n' << RESET;
    }


    return (0);
}