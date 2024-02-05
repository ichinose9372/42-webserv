#include "Server.hpp"

void Server::validateServers(const std::vector<Servers> &servers)
{
    if (servers.size() == 0)
    {
        throw std::runtime_error("No servers in config");
    }
    std::vector<Servers>::const_iterator it = servers.begin();
    for (; it != servers.end(); it++)
    {
        if (it->getPort() == 0)
            throw std::runtime_error("No port in config");
    }
}

void Server::handleDuplicatePort(size_t port, const Servers &server)
{
    std::multimap<int, Servers>::iterator map_itr = requestMap.begin();
    for (; map_itr != requestMap.end(); map_itr++)
    {
        if (map_itr->second.getPort() == port)
        {
            requestMap.insert(std::make_pair(map_itr->first, server));
            break;
        }
    }
}

void Server::initializeSocketAddress(size_t port)
{
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(port);
    addrlen = ADDRLEN;
}

void Server::initializeServerSocket(const Servers &server, size_t port)
{
    int socket_fd;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        close(socket_fd);
        throw std::runtime_error("Socket creation failed");
    }

    int opt = 1;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        close(socket_fd);
        throw std::runtime_error("setsockopt");
    }
    initializeSocketAddress(port);
    if (bind(socket_fd, (struct sockaddr *)&this->address, sizeof(this->address)) < 0)
    {
        close(socket_fd);
        throw std::runtime_error("bind out");
    }
    if (listen(socket_fd, 3) < 0)
    {
        close(socket_fd);
        throw std::runtime_error("listen");
    }

    // ソケットをノンブロッキングモードに設定
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1)
    {
        close(socket_fd);
        throw std::runtime_error("Failed to get flags for socket");
    }

    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        close(socket_fd);
        throw std::runtime_error("Failed to set socket to non-blocking mode");
    }
    requestMap.insert(std::make_pair(socket_fd, server));
    struct pollfd server_pollfd = {socket_fd, POLLIN, 0};
    this->pollfds.push_back(server_pollfd);
}

void Server::initializeServers(const std::vector<Servers> &servers)
{
    std::vector<size_t> ports;

    std::vector<Servers>::const_iterator it = servers.begin();
    for (; it != servers.end(); it++)
    {
        size_t port = it->getPort();
        if (std::find(ports.begin(), ports.end(), port) != ports.end())
        {
            handleDuplicatePort(port, *it);
            continue;
        }
        initializeServerSocket(*it, port);
        ports.push_back(port);
    }
}

Server::Server(const MainConfig &conf)
{
    std::vector<Servers> servers = conf.getServers();
    validateServers(servers);
    initializeServers(servers);
}

Server::~Server()
{
}

void Server::acceptNewConnection(int server_fd, std::vector<struct pollfd> &pollfds, struct sockaddr_in &address, int &addrlen)
{
    int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0)
    {
        close(server_fd);
        throw std::runtime_error("accept");
    }

    // 新しいソケットをノンブロッキングモードに設定
    int flags = fcntl(new_socket, F_GETFL, 0);
    if (flags == -1)
    {
        close(new_socket); // フラグの取得に失敗した場合は、新しいソケットを閉じます。
        throw std::runtime_error("Failed to get flags for new socket");
    }

    if (fcntl(new_socket, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        close(new_socket); // ノンブロッキングモードへの設定に失敗した場合は、新しいソケットを閉じます。
        throw std::runtime_error("Failed to set new socket to non-blocking mode");
    }
    struct pollfd new_socket_struct = {new_socket, POLLIN, 0};
    pollfds.push_back(new_socket_struct);
    requestStringMap.insert(std::make_pair(new_socket, ""));
}

bool Server::isTimeout(clock_t start)
{
    double time = Timer::calculateTime(start);
    return time > TIMEOUT;
}

// static int stringToInt(const std::string &str, bool &success)
// {
//     std::istringstream iss(str);
//     int number;
//     iss >> number;
// // static int stringToInt(const std::string &str, bool &success) {
// //     std::istringstream iss(str);
// //     int number;
// //     iss >> number;

// //     success = iss.good() || iss.eof();
// //     return success ? number : 0;
// // }

// // ヘッダをパースし、Content-Lengthの値を返す。
// // static int getContentLengthFromHeaders(const std::string &headers) {
// //     // ヘッダからContent-Lengthの値を見つけ、整数として返す疑似コード
// //     std::string contentLengthKeyword = "Content-Length: ";
// //     size_t startPos = headers.find(contentLengthKeyword);
// //     if (startPos != std::string::npos) {
// //         size_t endPos = headers.find("\r\n", startPos);
// //         std::string contentLengthValue = headers.substr(startPos + contentLengthKeyword.length(), endPos - (startPos + contentLengthKeyword.length()));
// //         bool conversionSuccess;
// //         int contentLength = stringToInt(contentLengthValue, conversionSuccess);
// //         if (conversionSuccess) {
// //             return contentLength;
// //         } else {
// //             std::cerr << "Content-Length conversion failed: invalid value" << std::endl;
// //         }
// //     }
// //     return -1; // Content-Lengthが見つからない場合
// // }

bool Server::receiveRequest(int socket_fd)
{
    int valread;
    char buffer[BUFFER_SIZE] = {0};
    valread = recv(socket_fd, buffer, BUFFER_SIZE, 0);
    if (valread > 0)
    {
        //読み込みが完了していない可能性があるので、次の読み込みを待つ
        //0の時は、読み込みが完全に終了していると判定してtrueを返す
        requestStringMap[socket_fd].append(buffer, valread);
        std::cout << "requestStringMap[socket_fd]: " << requestStringMap[socket_fd] << std::endl;
        //リクエストの中身から、Content-Lengthを取得する
        //その長さ分を読み込む
        if (requestStringMap[socket_fd].find("Content-Length:") != std::string::npos)
            return false;
        else if (requestStringMap[socket_fd].find("transfer-encoding: chunked") != std::string::npos)
            return false;
        else 
        {
            return true;
        }
    }
    else if (valread < 0)
    {
        close(socket_fd);
        deletePollfds(socket_fd);
        return false;
    }
    else//読み込みが完全に終了しているので、trueを返す
    {
        return true;
    }
}

Request Server::findServerandlocaitons(int socket_fd)
{
    (void)socket_fd;
    Request req(requestStringMap[socket_fd]);
    Servers server;
    bool foundServer = false; 

    std::multimap<int, Servers>::iterator it = requestMap.begin();
    for (; it != requestMap.end(); it++)
    {   
        // std::cout <<it->second.getServerNames() << " : " << req.getHost() << " : " << it->second.getPort() << " : " << req.getPort() << std::endl;
        std::stringstream ss(req.getPort());
        size_t port_num;
        ss >> port_num;
        if ((it->second.getServerNames() == req.getHost()) && (it->second.getPort() == port_num))
        { 

            server = it->second;
            foundServer = true;
            break;
        }
    }
    if (!foundServer)
    {
        req.setReturnParameter(404, "404.html");
        return req;
    }
    req.remakeRequest(server);
    return req;
}

bool Server::sendResponse(int socket_fd, Response &res)
{
    std::string response = res.getResponse();
    if (response.empty()) 
        response = "HTTP/1.1 204 No Content\r\n\r\n";
    else if (response.size() > MAX_RESPONSE_SIZE) 
        response = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/plain\r\n\r\nResponse too large.";
    ssize_t sendByte = send(socket_fd, response.c_str(), response.size(), MSG_NOSIGNAL); // Linuxの場合
    if (sendByte < 0) //senderrorの場合
    {
        close(socket_fd);
        deletePollfds(socket_fd);
        return true;
    }
    else
    {
        size_t sentSize = static_cast<size_t>(sendByte);
        if (sentSize < response.size())
        {
            std::string remainingResponse = response.substr(sendByte);
            res.setResponse(remainingResponse);
            return false;
        }
        else
            return true;
    }
}

void Server::processRequest(int socket_fd)
{
    Request req = findServerandlocaitons(socket_fd);
    Response res;
    this->responseConectionMap.insert(std::make_pair(socket_fd, res));
    Controller::processFile(req, this->responseConectionMap[socket_fd]);
}

void Server::recvandProcessConnection(struct pollfd &pfd)
{
    //読み込みが完全に終了したのかをあboolで確認する。
    bool recv_complete = receiveRequest(pfd.fd);
    if (recv_complete)
    {
        processRequest(pfd.fd);
        pfd.events = POLLOUT;
    }
    else //読み込みが完全に終了していない場合は、次の読み込みを待つ
        pfd.events = POLLIN;
    return ;
}

void Server::deletePollfds(int socket_fd)
{
    for (std::vector<struct pollfd>::iterator it = pollfds.begin(); it != pollfds.end(); ++it)
    {
        if (it->fd == socket_fd)
        {
            it = pollfds.erase(it);
            return ; //削除したら、ループを抜ける
        }
    }
}

void Server::sendConnection(struct pollfd &pfd)
{
    if (!sendResponse(pfd.fd, this->responseConectionMap[pfd.fd]))
        return ; //送信が完全に終了していない場合はreurtunして、次の送信を待つ
    close(pfd.fd);
    deletePollfds(pfd.fd);    // pollfdsから該当するエントリを削除
}

void Server::runEventLoop()
{
    size_t start_pollfds_size = pollfds.size();
    while (true)
    {
        if (poll(pollfds.data(), pollfds.size(), -1) > 0)
        {
            for (size_t i = 0; i < pollfds.size(); ++i)
            {
                if (i < start_pollfds_size && pollfds[i].revents & POLLIN)
                    acceptNewConnection(pollfds[i].fd, pollfds, address, addrlen);
                else if (pollfds[i].revents & POLLIN)
                {
                    //recv request　and process it;
                    recvandProcessConnection(pollfds[i]);
                }
                else if (pollfds[i].revents & POLLOUT)
                {
                    //send response;
                    sendConnection(pollfds[i]);
                }
            }
        }
        else
            throw std::runtime_error("Poll failed");
    }
}
