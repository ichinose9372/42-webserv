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
#include "Timer.hpp"
#include <map>

#define  BUFFER_SIZE 1024
#define  MAX_RESPONSE_SIZE 1000000 // 改善する予定
#define  TIMEOUT 5

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
        std::map<int, Request> requestConectionMap;
        std::map<int, Response> responseConectionMap;
        Server();
    public:
        Server(const MainConfig& conf);
        ~Server();
        void runEventLoop();
        void acceptNewConnection(int server_fd, std::vector<struct pollfd>& pollfds, struct sockaddr_in& address, int& addrlen);
        void recvandProcessConnection(struct pollfd& pfd);
        void sendConnection(struct pollfd& pfd);
        //initializefunctions
        void initializeServers(const std::vector<Servers>& servers);
        void validateServers(const std::vector<Servers>& servers);   
        void handleDuplicatePort(size_t port, const Servers& server);
        void initializeServerSocket(const Servers& server, size_t port);
        void initializeSocketAddress(size_t port);
        //request functions
        bool receiveRequest(int socket_fd, std::string& Request);
        void processRequest(int socket_fd, std::string& request);
        Request findServerandlocaitons(int socket_fd, const std::string& request);  
        Servers findServerBySocket(int socket_fd);
        bool isTimeout(clock_t start);
        // void sendTimeoutResponse(int socket_fd);
        //response functions
        bool sendResponse(int socket_fd, Response& res);
        //pollfds functions
        void deletePollfds(int socket_fd);

};

#endif
