#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <vector>
#include "../cnf/MainConfig.hpp"
#include "../http/Response.hpp"
#include "../http/Request.hpp"
#include "../http/Controller.hpp"

#define ADDRLEN sizeof(address)

class MainConfig;

//port は　config から読み込む

class Server
{
    private:
        int server_fd;
        struct sockaddr_in address;
        std::vector<struct pollfd> pollfds;
        int addrlen;
    public:
        Server();
        Server(const MainConfig& conf);
        ~Server();
        void acceptNewConnection(int server_fd, std::vector<struct pollfd>& pollfds, struct sockaddr_in& address, int& addrlen);
        void handleExistingConnection(struct pollfd& pfd);
        void runEventLoop();
};

#endif
