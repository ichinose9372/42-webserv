#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <vector>

class Server
{
    private:
        int server_fd;
        struct sockaddr_in address;
        std::vector<struct pollfd> pollfds;
    public:
        Server();
        ~Server();
};

#endif
