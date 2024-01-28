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
    struct pollfd new_socket_struct = {new_socket, POLLIN, 0};
    requestMap.insert(std::make_pair(new_socket, requestMap.find(server_fd)->second));
    pollfds.push_back(new_socket_struct);
}

bool Server::isTimeout(clock_t start)
{
    double time = Timer::calculateTime(start);
    return time > TIMEOUT;
}

static int stringToInt(const std::string &str, bool &success) {
    std::istringstream iss(str);
    int number;
    iss >> number;

    success = iss.good() || iss.eof();
    return success ? number : 0;
}

// ヘッダをパースし、Content-Lengthの値を返す。
static int getContentLengthFromHeaders(const std::string &headers) {
    // ヘッダからContent-Lengthの値を見つけ、整数として返す疑似コード
    std::string contentLengthKeyword = "Content-Length: ";
    size_t startPos = headers.find(contentLengthKeyword);
    if (startPos != std::string::npos) {
        size_t endPos = headers.find("\r\n", startPos);
        std::string contentLengthValue = headers.substr(startPos + contentLengthKeyword.length(), endPos - (startPos + contentLengthKeyword.length()));
        bool conversionSuccess;
        int contentLength = stringToInt(contentLengthValue, conversionSuccess);
        if (conversionSuccess) {
            return contentLength;
        } else {
            std::cerr << "Content-Length conversion failed: invalid value" << std::endl;
        }
    }
    return -1; // Content-Lengthが見つからない場合
}

bool Server::receiveRequest(int socket_fd, std::string &Request)
{
    int valread;
    char buffer[BUFFER_SIZE] = {0};
    clock_t start = Timer::startTimer();
    int contentLength = -1; // ボディサイズ
    int receivedLength = 0; // ボディの総受信データ数
    bool bodyFlg = false;     // 読み込みデータにボディが存在するかどうか
    bool headerNowFlg = true; // 読み込みデータがヘッダかどうか

    // valreadがBUFFER_SIZEと等しいか、もしくは0以上の場合ループを続ける
    while (true) {
        valread = recv(socket_fd, buffer, BUFFER_SIZE, 0);

        if (valread > 0)
        {
            // 受信したデータをリクエストに追加
            Request.append(buffer, valread);

            // ボディを持っているかつ読み込み内容がヘッダである場合、総受信データ数をカウント
            if (!headerNowFlg && bodyFlg)
                receivedLength += valread;

            // valreadの更新
            if (contentLength == -1)
            {
                // Content-Lengthの値をヘッダから取得する
                contentLength = getContentLengthFromHeaders(Request);
                if (contentLength != -1)
                    bodyFlg = true;
            }
            // 読み込み内容がヘッダかどうか
            if (headerNowFlg)
            {
                if (Request.find("\r\n\r\n") != std::string::npos)
                    headerNowFlg = false;
            }

            // 必要な量を受信したらループを終了
            if (!headerNowFlg && receivedLength >= contentLength)
                break;

            // タイムアウトチェック
            if (isTimeout(start))
                return true;

            // バッファをクリア
            memset(buffer, 0, BUFFER_SIZE);
        }
        else if (valread == 0)
        {
            if (isTimeout(start))
                return true;
            return false;
        }
        else
        {
            // if (errno == EWOULDBLOCK || errno == EAGAIN)
                continue; // データがまだ利用可能でない。後で再試行するためにループを継続する。
            close(socket_fd);
            throw std::runtime_error("Recv failed");
        }
    }

    // std::cout << "Request ===>>> " << Request << std::endl;
    return false;
}

Servers Server::findServerBySocket(int socket_fd)
{
    std::multimap<int, Servers>::iterator it = requestMap.begin();
    for (; it != requestMap.end(); it++)
    {
        if (it->first == socket_fd)
            return it->second;
    }
    throw std::runtime_error("Server not found");
}

Request Server::findServerandlocaitons(int socket_fd, const std::string &buffer)
{
    Request req(buffer);
    Servers server = findServerBySocket(socket_fd);
    if (req.getReturnParameter().first != 0)
    {
        return req;
    }
    req.remakeRequest(server);
    return req;
}

void Server::sendResponse(int socket_fd, Response &res)
{
    std::string response = res.getResponse();
    if (response.empty())
    {
        throw std::runtime_error("Response is empty");
    }
    else if (response.size() > MAX_RESPONSE_SIZE)
    {
        throw std::runtime_error("Response too large");
    }
    int status = send(socket_fd, response.c_str(), response.size(), SO_NOSIGPIPE);
    if (status == 0)
    {
        ;
    }
    else if (status < 0)
    {
        close(socket_fd);
        throw std::runtime_error("Send failed");
    }
}

void Server::sendTimeoutResponse(int socket_fd)
{
    Response res;
    res.setStatus("408 Request Timeout");
    res.setHeaders("Content-Type: ", "text/html");
    res.setBody("<html><body><h1>408 Request Timeout</h1></body></html>");
    res.setHeaders("Content-Length: ", std::to_string(res.getBody().size()));
    res.setResponse();
    sendResponse(socket_fd, res);
}

void Server::processRequestAndSendResponse(int socket_fd, std::string &request)
{
    Request req = findServerandlocaitons(socket_fd, request);
    Response res;
    Controller con;
    con.processFile(req, res);
    sendResponse(socket_fd, res);
}

void Server::handleExistingConnection(struct pollfd &pfd)
{
    std::string request;
    bool timeout = receiveRequest(pfd.fd, request);
    if (timeout)
        sendTimeoutResponse(pfd.fd);
    else
        processRequestAndSendResponse(pfd.fd, request);
    close(pfd.fd);
    // pollfdsから該当するエントリを削除
    for (std::vector<struct pollfd>::iterator it = pollfds.begin(); it != pollfds.end();)
    {
        if (it->fd == pfd.fd)
        {
            it = pollfds.erase(it);
            // エントリを削除した場合は、イテレータを進めない
        }
        else
        {
            ++it;
        }
    }
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
                    handleExistingConnection(pollfds[i]);
            }
        }
        else
            throw std::runtime_error("Poll failed");
    }
}
