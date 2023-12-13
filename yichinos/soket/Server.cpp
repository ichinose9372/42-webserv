#include "Server.hpp"

void Server::validateServers(const std::vector<Servers>& servers) 
{
    if (servers.size() == 0) {
        throw std::runtime_error("No servers in config");
    }
    std::vector<Servers>::const_iterator it = servers.begin();
    for(; it != servers.end(); it++)
    {
        if (it->getPort() == 0)
            throw std::runtime_error("No port in config");
    }
}

void Server::handleDuplicatePort(size_t port, const Servers& server)
{
    std::multimap<int, Servers>::iterator map_itr = requestMap.begin();
    for(; map_itr != requestMap.end(); map_itr++)
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

void Server::initializeServerSocket(const Servers& server, size_t port)
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

    requestMap.insert(std::make_pair(socket_fd, server));
    struct pollfd server_pollfd = {socket_fd, POLLIN, 0};
    this->pollfds.push_back(server_pollfd);

}


void Server::initializeServers(const std::vector<Servers>& servers)
{
    std::vector<size_t> ports;

    std::vector<Servers>::const_iterator it = servers.begin();
    for(; it != servers.end(); it++)
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


Server::Server(const MainConfig& conf)
{
    std::vector<Servers> servers = conf.getServers();
    validateServers(servers);
    initializeServers(servers);
}

Server::~Server()
{
    //
}

void Server::acceptNewConnection(int server_fd, std::vector<struct pollfd>& pollfds, struct sockaddr_in& address, int& addrlen) 
{
    int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        std::cerr << "Accept failed" << std::endl;
        return;
    }
    struct pollfd new_socket_struct = {new_socket, POLLIN, 0};
    requestMap.insert(std::make_pair(new_socket, requestMap.find(server_fd)->second));
    pollfds.push_back(new_socket_struct);
    // std::cout << "accept new connection" << std::endl;  
}

void Server::receiveRequest(int socket_fd, char** buffer)
{
    int valread;
    int buffer_size = BUFFER_SIZE;
    memset(*buffer, 0, buffer_size);
    while ((valread = read(socket_fd, *buffer, buffer_size)) == buffer_size) 
    {
        char *tmp;
        tmp = *buffer;
        if (buffer_size * 2 > 1000000)
            throw std::runtime_error("Request too large");
        char * new_buffer = new char[buffer_size * 2];
        memcpy(new_buffer, *buffer, buffer_size);
        *buffer = new_buffer;
        delete[] tmp;
        buffer_size *= 2;
    }
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



Request Server::processRequest(int socket_fd, const char* buffer) 
{
    Request req(buffer);
    Servers server = findServerBySocket(socket_fd);
    //fd から server を探して、その server に対応する location を探す
    req.remakeRequest(server);
    return req;
}

void Server::sendResponse(int socket_fd, Response& res) 
{
    std::string response = res.getResponse();
    // std::cout << "response size =  " << response.size() << " \n -----response------  \n " << response << std::endl;
    if (response.size() == 0)
    {
        throw std::runtime_error("Response is empty");
    }
    else if (response.size() > 1000000)
    {
        throw std::runtime_error("Response too large");
    }
    send(socket_fd, response.c_str(), response.size(), 0);
    // std::cout << "----send finish-------" << std::endl;
}

void Server::handleExistingConnection(struct pollfd& pfd) 
{
    // std::cout << "IN handleExistingConnection" << std::endl;
    char * buffer = new char[BUFFER_SIZE];
    receiveRequest(pfd.fd, &buffer);
    Request req = processRequest(pfd.fd, buffer);
    Response res;
    Controller con;
    con.processFile(req, res);
    sendResponse(pfd.fd, res);
    delete[] buffer;
    close(pfd.fd);
}

void Server::runEventLoop()
{
    size_t start_pollfds_size = pollfds.size();
    //print all pollfds
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
    }
}
