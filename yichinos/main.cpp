#include "MainConfig.hpp"
#include <iostream>
#include <string>
#include <exception>


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        return (0);
    }
    try
    {
        std::string filename;
        filename = argv[1];
        MainConfig myconf(filename);
        myconf.parseLine();//make tokens
        myconf.tokenSearch();//make servers
    }
    catch(const std::exception& e)
    {
        std::cerr << RED << e.what() << '\n' << RESET;
    }


    return (0);
}