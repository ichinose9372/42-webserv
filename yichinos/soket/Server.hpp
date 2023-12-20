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
#include "../cnf/Servers.hpp"
#include <map>

#define  BUFFER_SIZE 1024

#define ADDRLEN sizeof(address)

class MainConfig;
class Servers;

//port は　config から読み込む

class Server
{
    private:
        size_t server_fd;
        struct sockaddr_in address;
        std::vector<struct pollfd> pollfds;
        int addrlen;
        std::multimap<int , Servers> requestMap;
        Server();
    public:
        Server(const MainConfig& conf);
        ~Server();
        void acceptNewConnection(int server_fd, std::vector<struct pollfd>& pollfds, struct sockaddr_in& address, int& addrlen);
        void handleExistingConnection(struct pollfd& pfd);
        void runEventLoop();

        //initializefunctions
        void initializeServers(const std::vector<Servers>& servers);
        void validateServers(const std::vector<Servers>& servers);   
        void handleDuplicatePort(size_t port, const Servers& server);
        void initializeServerSocket(const Servers& server, size_t port);
        void initializeSocketAddress(size_t port);
        //request functions
        void receiveRequest(int socket_fd, std::string &Request);
        Request processRequest(int socket_fd, const std::string& request);  
        Servers findServerBySocket(int socket_fd);
        //response functions
        void sendResponse(int socket_fd, Response& res);

};

#endif
