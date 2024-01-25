#include "socket/Server.hpp"
#include "cnf/MainConfig.hpp"
#include "signal/Signal.hpp"


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        return (0);
    }
    try
    {
        ignore_sig();
        std::string filename;
        filename = argv[1];
        MainConfig myconf(filename);
        Server server(myconf);
        std::cout << "Server is running..." << std::endl;
        while (true)
        {
            server.runEventLoop();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << RED << e.what() << '\n' << RESET;
    }
    return 0;
}