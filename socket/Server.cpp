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

static int stringToInt(const std::string &str, bool &success)
{
    std::istringstream iss(str);
    int number;
    iss >> number;

    success = iss.good() || iss.eof();
    return success ? number : 0;
}

// // ヘッダをパースし、Content-Lengthの値を返す。
static int getContentLengthFromHeaders(const std::string &headers)
{
    // ヘッダからContent-Lengthの値を見つけ、整数として返す疑似コード
    std::string contentLengthKeyword = "Content-Length: ";
    size_t startPos = headers.find(contentLengthKeyword);
    if (startPos != std::string::npos)
    {
        size_t endPos = headers.find("\r\n", startPos);
        std::string contentLengthValue = headers.substr(startPos + contentLengthKeyword.length(), endPos - (startPos + contentLengthKeyword.length()));
        bool conversionSuccess;
        int contentLength = stringToInt(contentLengthValue, conversionSuccess);
        if (conversionSuccess)
        {
            return contentLength;
        }
        else
        {
            std::cerr << "Content-Length conversion failed: invalid value" << std::endl;
        }
    }
    return -1; // Content-Lengthが見つからない場合
}

void Server::processReceivedHeaders(int socket_fd, const std::string &headers)
{
    std::string requestLine = headers.substr(0, headers.find("\r\n"));
    std::string method = requestLine.substr(0, requestLine.find(" "));

    if (method != "GET")
    {
        if (headers.find("Content-Length:") == std::string::npos && headers.find("Transfer-Encoding: chunked") == std::string::npos)
        {
            std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nRequired header not found.";
            send(socket_fd, response.c_str(), response.length(), 0);
            close(socket_fd);
            deletePollfds(socket_fd);
            throw std::runtime_error("Required header not found");
        }
        isBodyFlg[socket_fd] = true;
    }
    else
    {
        isBodyFlg[socket_fd] = false;
    }
    if (headers.find("Content-Length:") != std::string::npos)
    {
        this->recvContentLength[socket_fd] = getContentLengthFromHeaders(headers);
    }
}

void Server::initReceiveFlg(int socket_fd)
{
    isBodyFlg[socket_fd] = false;
    isNowHeaderFlg[socket_fd] = false;
}

int Server::receiveRequest(int socket_fd)
{
    if (this->recvContentLength.find(socket_fd) == this->recvContentLength.end())
        this->recvContentLength.insert(std::make_pair(socket_fd, 0));
    int valread;
    char buffer[BUFFER_SIZE] = {0};
    valread = recv(socket_fd, buffer, BUFFER_SIZE, 0);
    std::cout << "valread: " << valread << std::endl;
    if (valread > 0)
    {
        std::cout << "--- recv --- " << std::endl;
        std::cout << buffer << std::endl;
        std::cout << "--- recv --- " << std::endl;

        requestStringMap[socket_fd].append(buffer, valread);

        // ヘッダー終端を探す（リクエストが完全にヘッダーを受信したかを確認するため）
        size_t headerEndPos = requestStringMap[socket_fd].find("\r\n\r\n");
        if (!isNowHeaderFlg[socket_fd] && headerEndPos != std::string::npos)
        {
            isNowHeaderFlg[socket_fd] = true;
            // ヘッダーが完全に受信された後に、リクエストメソッドを確認
            std::string headers = requestStringMap[socket_fd].substr(0, headerEndPos);
            std::string requestLine = headers.substr(0, headers.find("\r\n"));
            std::string method = requestLine.substr(0, requestLine.find(" "));

            // GETメソッドの場合はContent-LengthまたはTransfer-Encodingのチェックをスキップ
            if (method != "GET" && method != "HEAD")
            {
                std::cout << "method: " << method << std::endl;
                if (headers.find("Content-Length:") == std::string::npos && headers.find("Transfer-Encoding: chunked") == std::string::npos)
                {
                    isBodyFlg[socket_fd] = false;
                    // // エラーレスポンスを送信
                    // std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nRequired header not found.";
                    // send(socket_fd, response.c_str(), response.length(), 0);
                    // // close(socket_fd);
                    // // deletePollfds(socket_fd);
                    // initReceiveFlg(socket_fd);
                    // return OPERATION_ERROR; // エラーコードを返す
                }
                else
                    isBodyFlg[socket_fd] = true;
            }
            else
            {
                isBodyFlg[socket_fd] = false;
            }
            if (headers.find("Content-Length:") != std::string::npos)
            {
                this->recvContentLength[socket_fd] = getContentLengthFromHeaders(requestStringMap[socket_fd]);
                return RETRY_OPERATION;
            }
        }
        // receiveRequestメソッド内でchunked転送の処理を追加
        if (requestStringMap[socket_fd].find("Transfer-Encoding: chunked") != std::string::npos)
        {
            // チャンクデータの読み取りと処理
            std::string &reqData = requestStringMap[socket_fd];
            size_t pos = 0;
            while (pos < reqData.size())
            {
                // チャンクサイズの読み取り
                size_t endPos = reqData.find("\r\n", pos);
                if (endPos == std::string::npos)
                    break; // ヘッダの終わりが見つからない場合

                std::string chunkSizeStr = reqData.substr(pos, endPos - pos);
                unsigned int chunkSize = std::stoi(chunkSizeStr, nullptr, 16);
                pos = endPos + 2; // "\r\n"をスキップ

                if (chunkSize == 0)
                {
                    // 最後のチャンクを示す（トレイラーの処理が必要な場合はここで行う）
                    break;
                }

                // チャンクデータの取得（chunkSizeが次のチャンクのサイズ）
                std::string chunkData = reqData.substr(pos, chunkSize);
                // ここでchunkDataを適切に処理する（例えば、結合してリクエストボディを形成する）

                pos += chunkSize + 2; // チャンクデータと次の"\r\n"をスキップ
            }

            // チャンク読み取りが完了したら、適切な処理を行う
            // 例: リクエストの処理、レスポンスの生成など
            initReceiveFlg(socket_fd);
            return OPERATION_DONE;
        }

        this->totalSamread[socket_fd] += valread;
        std::cout << "isNowHeaderFlg[socket_fd]: " << isNowHeaderFlg[socket_fd] << std::endl;
        std::cout << "isBodyFlg[socket_fd]: " << isBodyFlg[socket_fd] << std::endl;
        if (isNowHeaderFlg[socket_fd] && ((!isBodyFlg[socket_fd]) || (this->recvContentLength[socket_fd] != 0 && this->totalSamread[socket_fd] >= this->recvContentLength[socket_fd])))
        {
            std::cout << "---- Normal Done --- " << std::endl;
            this->recvContentLength.erase(socket_fd);
            this->totalSamread.erase(socket_fd);
            initReceiveFlg(socket_fd);
            return OPERATION_DONE;
        }
        // if (requestStringMap[socket_fd].find("Content-Length:") != std::string::npos)
        // {
        //     this->recvContentLength[socket_fd] = getContentLengthFromHeaders(requestStringMap[socket_fd]);
        //     return RETRY_OPERATION;
        // }
        // else if (requestStringMap[socket_fd].find("transfer-encoding: chunked") != std::string::npos)
        // {
        //     return RETRY_OPERATION;
        // }
        // else
        // {
        //     std::cout << "---- END Done --- " << std::endl;
        //     return OPERATION_DONE;
        // }
        std::cout << "RETRY!" << std::endl;
        return RETRY_OPERATION;
    }
    else if (valread < 0)
    {
        initReceiveFlg(socket_fd);
        return OPERATION_ERROR;
    }
    else // 読み込みが完全に終了しているので、trueを返す
    {
        std::cout << "FINAL END" << std::endl;
        initReceiveFlg(socket_fd);
        return OPERATION_DONE;
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
    if (!foundServer && req.getReturnParameter().first == 0)
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
        return false;
    else if (response.size() > MAX_RESPONSE_SIZE)
        response = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/plain\r\n\r\nResponse too large.";
    ssize_t sendByte = send(socket_fd, response.c_str(), response.size(), 0); // Linuxの場合
    if (sendByte < 0)
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
    int recv_result = receiveRequest(pfd.fd);
    if (recv_result == OPERATION_DONE)
    {
        processRequest(pfd.fd);
        // もしfdのレスポンスクラスにぱパイプのfdがセットされてたら、そのfdをpollfdsに追加してPOLLINを監視する　=もしCGIだったら
        if (this->responseConectionMap[pfd.fd].getCGIreadfd() != -1)
        {
            struct pollfd pipe_pollfd = {this->responseConectionMap[pfd.fd].getCGIreadfd(), POLLIN, 0};
            this->pollfds.push_back(pipe_pollfd);
            this->cgiReadFdMap.insert(std::make_pair(this->responseConectionMap[pfd.fd].getCGIreadfd(), pfd.fd)); // 6 5
            pfd.events = 0;
        }
        else
        {
            pfd.events = POLLOUT;
        }
    }
    else if (recv_result == RETRY_OPERATION)
    {
        pfd.events = POLLIN;
    }
    else
    {
        close(pfd.fd);
        deletePollfds(pfd.fd);
    }
    return;
}

void Server::deletePollfds(int socket_fd)
{
    for (std::vector<struct pollfd>::iterator it = pollfds.begin(); it != pollfds.end(); ++it)
    {
        if (it->fd == socket_fd)
        {
            it = pollfds.erase(it);
            break;
        }
    }
    requestStringMap.erase(socket_fd);
    responseConectionMap.erase(socket_fd);
}

void Server::sendConnection(struct pollfd &pfd)
{
    if (!sendResponse(pfd.fd, this->responseConectionMap[pfd.fd]))
        return; // 送信が完全に終了していない場合はreurtunして、次の送信を待つ
    close(pfd.fd);
    deletePollfds(pfd.fd); // pollfdsから該当するエントリを削除
}

void Server::changePollfds(int changefd, short events)
{
    std::vector<struct pollfd>::iterator it = pollfds.begin();
    for (; it != pollfds.end(); it++)
    {
        if (it->fd == changefd)
        {
            it->events = events;
            break;
        }
    }
}

void Server::server_Setresponse(int fd, const std::string &status, const std::string &contentType, const std::string &body)
{
    this->responseConectionMap[fd].setStatus(status);
    this->responseConectionMap[fd].setHeaders("Content-Type: ", contentType);
    this->responseConectionMap[fd].setHeaders("Content-Length: ", Utils::my_to_string(body.size()));
    this->responseConectionMap[fd].setBody(body);
    this->responseConectionMap[fd].setResponse();
}

void Server::readCgiOutput(struct pollfd &pfd)
{
    char buf[BUFFER_SIZE];
    std::string body;
    int requestfd = this->cgiReadFdMap[pfd.fd];
    int pipefd = pfd.fd;
    // read from pipe
    int len = read(pipefd, buf, sizeof(buf) - 1);
    if (len > 0)
    {
        buf[len] = '\0';
        body = this->responseConectionMap[requestfd].getBody();
        body.append(buf);
        this->responseConectionMap[requestfd].setBody(body);
        changePollfds(pipefd, POLLIN);
        return;
    }
    else if (len == 0) // EOF
    {
        server_Setresponse(requestfd, "200 OK", "text/html", this->responseConectionMap[requestfd].getBody());
        this->responseConectionMap[requestfd].setCGIreadfd(-1);
        changePollfds(requestfd, POLLOUT);
        close(pipefd);
        deletePollfds(pipefd);
        this->cgiReadFdMap.erase(pipefd);
        return;
    }
    else if (len < 0)
    {
        close(this->responseConectionMap[pfd.fd].getCGIreadfd());
        deletePollfds(this->responseConectionMap[pfd.fd].getCGIreadfd());
        this->cgiReadFdMap.erase(pfd.fd);
        server_Setresponse(pfd.fd, "500 Internal Server Error", "text/html", "<html><body><h1>500 Internal Server Error</h1><p>サーバーで内部エラーが発生しました。</p></body></html>");
        pollfds[this->cgiReadFdMap[pipefd]].events = POLLOUT;
    }
    return;
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
                if (pollfds[i].revents & POLLIN)
                {
                    if (this->cgiReadFdMap.find(pollfds[i].fd) != this->cgiReadFdMap.end())
                    {
                        readCgiOutput(pollfds[i]);
                    }
                    else if (i < start_pollfds_size)
                    {
                        acceptNewConnection(pollfds[i].fd, pollfds, address, addrlen);
                    }
                    else
                    {
                        recvandProcessConnection(pollfds[i]);
                    }
                }
                else if (pollfds[i].revents & POLLHUP)
                {
                    // pipeのfdが空で読み込まれない時に、POLLHUPが発生する
                    // pipeのfdを閉じる処理と、pollfdsから削除する処理を行う
                    // リクエストのfdをPOLLOUTに変更する
                    server_Setresponse(this->cgiReadFdMap[pollfds[i].fd], "200 OK", "text/html", this->responseConectionMap[this->cgiReadFdMap[pollfds[i].fd]].getBody());
                    this->responseConectionMap[this->cgiReadFdMap[pollfds[i].fd]].setCGIreadfd(-1);
                    changePollfds(this->cgiReadFdMap[pollfds[i].fd], POLLOUT);
                    close(pollfds[i].fd);
                    deletePollfds(pollfds[i].fd);
                    this->cgiReadFdMap.erase(pollfds[i].fd);
                }
                else if (pollfds[i].revents & POLLOUT)
                {
                    // レスポンス送信処理
                    sendConnection(pollfds[i]);
                }
            }
        }
        else
            throw std::runtime_error("Poll failed");
    }
}