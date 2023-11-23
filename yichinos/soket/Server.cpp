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


Server::~Server()
{
    close(this->server_fd);
}

void Server::acceptNewConnection(int server_fd, std::vector<struct pollfd>& pollfds, struct sockaddr_in& address, int& addrlen) {
    int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        std::cerr << "Accept failed" << std::endl;
        return;
    }
    std::cout << "New connection: " << new_socket << std::endl;
    struct pollfd new_socket_struct = {new_socket, POLLIN, 0};
    pollfds.push_back(new_socket_struct);
}

void Server::handleExistingConnection(struct pollfd& pfd) 
{
    std::string request;
    char buffer[1024];
    int valread;

    // while ((valread = read(pfd.fd, buffer, 1024)) > 0) 
    // {
    //     request.append(buffer, valread);
    //     if (request.find("\n") != std::string::npos) {
    //         break;
    //     }
    // }
    //read 
    valread = read(pfd.fd, buffer, 1024);
    request.append(buffer, valread);
    if (request.empty())
    {
        std::cout << "Connection closed: " << pfd.fd << std::endl;
        close(pfd.fd);
        pfd.fd = -1;
        return;
    }
    std::cout << "Request: " << request << std::endl;
    //use request to get response

    //write
    //404 not found htmlfile
    std::string body = "<html><body><h1>404 Not Found</h1></body></html>";
    std::string response = "HTTP/1.1 404 Not Found\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;
    send(pfd.fd, response.c_str(), response.size(), 0);
}

void Server::runEventLoop() {
    while (true) {
        if (poll(pollfds.data(), pollfds.size(), -1) > 0) {
            for (size_t i = 0; i < pollfds.size(); ++i) {
                if (pollfds[i].revents & POLLIN) {
                    if (i == 0) {
                        acceptNewConnection(server_fd, pollfds, address, addrlen);
                    } else {
                        handleExistingConnection(pollfds[i]);
                    }
                }
            }
        }
    }
}
