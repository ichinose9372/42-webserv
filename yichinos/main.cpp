#include "soket/Server.hpp"

int main()
{
    Server server;
    while (true)
    {
        server.runEventLoop();
        
    }
    return 0;
}