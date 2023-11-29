#include "Server.hpp"

Server::Server()
{
    // Creating socket file descriptor
    if ((this->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
    {
        std::cerr << "Setsockopt failed: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(8081);
    addrlen = ADDRLEN;
    
    // Forcefully attaching socket to the port 8080
    if (bind(this->server_fd, (struct sockaddr *)&this->address, sizeof(this->address)) < 0)
    {
        std::cerr << "bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (listen(this->server_fd, 3) < 0)
    {
        std::cerr << "listen" << std::endl;
        exit(EXIT_FAILURE);
    }
     struct pollfd server_pollfd = {server_fd, POLLIN, 0};
    this->pollfds.push_back(server_pollfd);
}

Server::Server(const MainConfig& conf)
{
    std::vector<Servers> servers = conf.getServers();
    if (servers.size() == 0) {
        throw std::runtime_error("No servers in config");
    }
    std::vector<Servers>::iterator it = servers.begin();
    std::vector<size_t> ports;
    int soket_fd;
    for(; it != servers.end(); it++)
    {
        if (it->getPort() == 0)
            throw std::runtime_error("No port in config");
        size_t port = it->getPort();
        if (std::find(ports.begin(), ports.end(), port) != ports.end()) 
        {
            // std::cout << requestMap.size() << std::endl;
            std::multimap<int, Servers>::iterator map_itr;
            for (map_itr = requestMap.begin(); map_itr != requestMap.end(); map_itr++)
            {
                std::cout << map_itr->second.getPort() << std::endl;
                if (map_itr->second.getPort() == port)
                {
                    requestMap.insert(std::make_pair(map_itr->first, *it));
                    break;
                }
            }
            // requestMap.insert(std::make_pair(soket_fd, *it));
            continue;
        }
        ports.push_back(port);
        if ((soket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            close(soket_fd);
            std::cerr << "socket failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        int opt = 1;
        if (setsockopt(soket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
        {
            std::cerr << "Setsockopt failed: " << strerror(errno) << std::endl;
            close(soket_fd);
            exit(EXIT_FAILURE);
        }

        this->address.sin_family = AF_INET;
        this->address.sin_addr.s_addr = INADDR_ANY;
        this->address.sin_port = htons(port);
        addrlen = ADDRLEN;
        if (bind(soket_fd, (struct sockaddr *)&this->address, sizeof(this->address)) < 0)
        {
            //all soket close
            //
            std::cerr << "bind failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (listen(soket_fd, 3) < 0)
        {
            //all soket close
            //
            std::cerr << "listen" << std::endl;
            exit(EXIT_FAILURE);
        }
        requestMap.insert(std::make_pair(soket_fd, *it));
        struct pollfd server_pollfd = {soket_fd, POLLIN, 0};
        this->pollfds.push_back(server_pollfd);
    }
    // std::cout << requestMap.size() << " : 3 "<< std::endl;
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
}

void Server::handleExistingConnection(struct pollfd& pfd) 
{
    std::string request;
    char buffer[1024];
    int valread;
    memset(buffer, 0, 1024);
    valread = read(pfd.fd, buffer, 1024);
    if (valread == 0) {
        std::cout << "Connection closed: " << pfd.fd << std::endl;
        std::cout << "-----------------------------------\n";
        close(pfd.fd);
        pfd.fd = -1;
        return;
    }
    Request req(buffer);
    Servers server = requestMap.find(pfd.fd)->second;
    std::multimap<int, Servers>::iterator it = requestMap.begin();
    for (; it != requestMap.end(); it++)
    {
        if (it->second.getServerNames() == req.getHost())
        {
            server = it->second;
        }
    }
    req.remakeRequest(server);
    Response res;
    Controller con;
    con.processFile(req, res);
    std::string response = res.getResponse();
    if (response.size() == 0) {
        throw std::runtime_error("Response is empty");
    }    
    send(pfd.fd, response.c_str(), response.size(), 0);
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
                {
                    acceptNewConnection(pollfds[i].fd, pollfds, address, addrlen);
                } 
                else if (pollfds[i].revents & POLLIN) 
                {
                    handleExistingConnection(pollfds[i]);
                }
            }
        }
    }
}
