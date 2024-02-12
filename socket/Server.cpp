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
    if (listen(socket_fd, 128) < 0) // 第2引数で待ち受けキューの長さを指定
    {
        close(socket_fd);
        throw std::runtime_error("listen");
    }
    this->listeningSockets.push_back(socket_fd);

    // ソケットをノンブロッキングモードに設定
    // int flags = fcntl(socket_fd, F_GETFL, 0);
    // if (flags == -1)
    // {
    //     close(socket_fd);
    //     throw std::runtime_error("Failed to get flags for socket");
    // }

    if (fcntl(socket_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC))
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
    // int flags = fcntl(new_socket, F_GETFL, 0);
    // if (flags == -1)
    // {
    //     close(new_socket); // フラグの取得に失敗した場合は、新しいソケットを閉じます。
    //     throw std::runtime_error("Failed to get flags for new socket");
    // }

    if (fcntl(new_socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC))
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

// recvで取得したデータからボディのサイズを取得する関数
static int getBodySize(const std::string& requestData) {
    // ヘッダーとボディの区切りを示すパターン
    const std::string delimiter = "\r\n\r\n";
    // 区切りの位置を検索
    size_t delimiterPos = requestData.find(delimiter);

    if (delimiterPos != std::string::npos) {
        // 区切りが見つかった場合、その位置+区切り文字列の長さをヘッダーの終わりとする
        size_t headerEndPos = delimiterPos + delimiter.length();
        // ボディのサイズは、リクエストデータの全長からヘッダーの終わりの位置を引いたもの
        return static_cast<int>(requestData.length() - headerEndPos);
    }

    // ヘッダーとボディの区切りが見つからない場合、ボディは存在しないとみなし、サイズは0とする
    return 0;
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

int Server::processChunkedRequest(int socket_fd)
{
    // 終了チャンク「0\r\n\r\n」が含まれているかを確認
    if (requestStringMap[socket_fd].find("0\r\n\r\n") != std::string::npos)
    {
        return OPERATION_DONE; // 終了チャンクが見つかった場合
    }
    else
    {
        return RETRY_OPERATION; // それ以外の場合は更にデータを受信する必要がある
    }
}

void Server::initReceiveFlg(int socket_fd)
{
    isBodyFlg[socket_fd] = false;
    isNowHeaderFlg[socket_fd] = false;
    isChunkedFlg[socket_fd] = false;
    this->recvContentLength.erase(socket_fd);
}

int Server::receiveRequest(int socket_fd)
{
    if (this->recvContentLength.find(socket_fd) == this->recvContentLength.end())
        this->recvContentLength.insert(std::make_pair(socket_fd, 0));
    int valread;
    char buffer[BUFFER_SIZE] = {0};
    valread = recv(socket_fd, buffer, BUFFER_SIZE, 0);
    if (valread > 0)
    {
        requestStringMap[socket_fd].append(buffer, valread);
        int currentBodySize = getBodySize(requestStringMap[socket_fd]);
        if (isChunkedFlg[socket_fd])
        {
            // チャンクデータの処理を行う
            int chunkedStat = processChunkedRequest(socket_fd);
            if (chunkedStat == 1)
            {
                return OPERATION_DONE;
            }
            else if (chunkedStat == 0)
            {
                return RETRY_OPERATION;
            }
            else
            {
                return OPERATION_ERROR;
            }
        }

        // ヘッダー終端を探す（リクエストが完全にヘッダーを受信したかを確認するため）
        size_t headerEndPos = requestStringMap[socket_fd].find("\r\n\r\n");
        // header読み込み処理
        if (!isNowHeaderFlg[socket_fd] && headerEndPos != std::string::npos)
        {
            isNowHeaderFlg[socket_fd] = true;
            // ヘッダーが完全に受信された後に、リクエストメソッドを確認
            std::string headers = requestStringMap[socket_fd].substr(0, headerEndPos);
            std::string requestLine = headers.substr(0, headers.find("\r\n"));
            std::string method = requestLine.substr(0, requestLine.find(" "));

            // GETメソッドの場合はContent-LengthまたはTransfer-Encodingのチェックをスキップ
            if (method != "GET" && method != "HEAD" && method != "DELETE")
            {
                if (headers.find("Content-Length:") == std::string::npos && headers.find("Transfer-Encoding: chunked") == std::string::npos)
                    isBodyFlg[socket_fd] = false;
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
                if (currentBodySize >= this->recvContentLength[socket_fd])
                    return OPERATION_DONE;
                else
                    return RETRY_OPERATION;
            }
            if (headers.find("Transfer-Encoding: chunked") != std::string::npos)
            {
                isChunkedFlg[socket_fd] = true;
                return RETRY_OPERATION;
            }
        } // header読み込み処理、終了

        if (isNowHeaderFlg[socket_fd] && ((!isBodyFlg[socket_fd]) || (this->recvContentLength[socket_fd] != 0 && currentBodySize >= this->recvContentLength[socket_fd])))
        {
            return OPERATION_DONE;
        }
        return RETRY_OPERATION;
    }
    else if (valread < 0)
    {
        return OPERATION_ERROR;
    }
    else // 読み込みが完全に終了しているので、trueを返す
    {
        return OPERATION_DONE;
    }
}

Request Server::findServerandlocaitons(int socket_fd)
{
    int bodySize = getBodySize(requestStringMap[socket_fd]);
    Request req(requestStringMap[socket_fd], bodySize);
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
    {
        res.setResponse();
        return false;
    }
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
        {
            return true;
        }
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
        initReceiveFlg(pfd.fd);
        processRequest(pfd.fd);
        // もしfdのレスポンスクラスにぱパイプのfdがセットされてたら、そのfdをpollfdsに追加してPOLLINを監視する　=もしCGIだったら
        if (this->responseConectionMap[pfd.fd].getCGIreadfd() != -1 || this->responseConectionMap[pfd.fd].getStatus() == "0")
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
        initReceiveFlg(pfd.fd);
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
    while (true)
    {
        if (poll(pollfds.data(), pollfds.size(), -1) > 0)
        {
            Timer::busyWaitMilliseconds(1);
            for (size_t i = 0; i < pollfds.size(); ++i)
            {
                if (pollfds[i].revents & POLLIN)
                {
                    if (this->cgiReadFdMap.find(pollfds[i].fd) != this->cgiReadFdMap.end())
                    {
                        readCgiOutput(pollfds[i]);
                    }
                    else if (std::find(listeningSockets.begin(), listeningSockets.end(), pollfds[i].fd) != listeningSockets.end())
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
