#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <vector>

#define ADDRLEN sizeof(address)

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
        ~Server();
        void acceptNewConnection(int server_fd, std::vector<struct pollfd>& pollfds, struct sockaddr_in& address, int& addrlen);
        void handleExistingConnection(struct pollfd& pfd);
        void runEventLoop();
};

#endif
