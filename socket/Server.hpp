#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <vector>
#include <algorithm>
#include "../cnf/MainConfig.hpp"
#include "../http/Response.hpp"
#include "../http/Request.hpp"
#include "../http/Controller.hpp"
#include "../cnf/Servers.hpp"
#include "Timer.hpp"
#include <map>

#define BUFFER_SIZE 1024
#define MAX_RESPONSE_SIZE 1000000 // 改善する予定
#define TIMEOUT 5
#define OPERATION_DONE 1
#define RETRY_OPERATION 0
#define OPERATION_ERROR -1
#define ADDRLEN sizeof(address)

class MainConfig;
class Servers;

// port は　config から読み込む

class Server
{
private:
    size_t server_fd;
    struct sockaddr_in address;
    std::vector<struct pollfd> pollfds;
    int addrlen;
    std::multimap<int, Servers> requestMap;
    std::map<int, std::string> requestStringMap;
    std::map<int, Response> responseConectionMap;
    std::map<int, int> cgiReadFdMap;
    std::map<int, int> recvContentLength;
    std::map<int, bool> isBodyFlg;
    std::map<int, bool> isNowHeaderFlg;
    std::map<int, bool> isChunkedFlg;
    std::vector<int> listeningSockets;
    Server();

public:
    Server(const MainConfig &conf);
    ~Server();
    void runEventLoop();
    void acceptNewConnection(int server_fd, std::vector<struct pollfd> &pollfds, struct sockaddr_in &address, int &addrlen);
    void recvandProcessConnection(struct pollfd &pfd);
    void sendConnection(struct pollfd &pfd);
    // initializefunctions
    void initializeServers(const std::vector<Servers> &servers);
    void validateServers(const std::vector<Servers> &servers);
    void handleDuplicatePort(size_t port, const Servers &server);
    void initializeServerSocket(const Servers &server, size_t port);
    void initializeSocketAddress(size_t port);
    // request functions
    int receiveRequest(int socket_fd);
    int processChunkedRequest(int socket_fd);
    std::string extractChunkedBodyFromRequest(int socket_fd);
    void initReceiveFlg(int socket_fd);
    void processRequest(int socket_fd);
    void processReceivedHeaders(int socket_fd, const std::string &headers);
    Request findServerandlocaitons(int socket_fd);
    Servers findServerBySocket(int socket_fd);
    bool isTimeout(clock_t start);
    // void sendTimeoutResponse(int socket_fd);
    // response functions
    bool sendResponse(int socket_fd, Response &res);
    void server_Setresponse(int fd, const std::string &status, const std::string &contentType, const std::string &body);
    void readCgiOutput(struct pollfd &pfd);
    // pollfds functions
    void deletePollfds(int socket_fd);
    void changePollfds(int changefd, short events);
};

#endif
