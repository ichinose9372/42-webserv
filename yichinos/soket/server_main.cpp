#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>

int main() {
    int server_fd;
    // ソケットの作成
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }
    std::cout << "soket  : " << server_fd << std::endl;

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
    {
        std::cerr << "Setsockopt failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return -1;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address)); // 構造体の初期化
    address.sin_family = AF_INET;         // アドレスファミリー: IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 任意のインターフェースを指定
    address.sin_port = htons(8081);       // ポート番号: 8081
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        std::cerr << strerror(errno) << std::endl;
        close(server_fd);
        return -1;
    }
    // リスニングを開始
    if (listen(server_fd, 3) < 0) {
        std::cerr << strerror(errno)<< std::endl;
        return -1;
    }

    int new_socket;
    int addrlen = sizeof(address);
    std::vector<struct pollfd> pollfds;

    struct pollfd server_pollfd = {server_fd, POLLIN, 0};
    pollfds.push_back(server_pollfd);
    // pollfds.push_back({server_fd, POLLIN, 0});

    std::cout << "Waiting for connections ..." << std::endl;
    while (true) 
    {
        if (poll(&pollfds[0], pollfds.size(), -1) > 0)
        {
            for (size_t i = 0; i < pollfds.size(); ++i)
            {
                if (pollfds[i].revents & POLLIN) 
                {
                    if (i == 0) 
                    {
                        // 新しい接続の処理
                        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                        if (new_socket < 0) 
                        {
                            std::cerr << "Accept failed" << std::endl;
                            continue;
                        }
                        std::cout << "New connection: " << new_socket << std::endl;
                        struct pollfd new_socket_struct = {new_socket, POLLIN, 0};
                        pollfds.push_back(new_socket_struct);
                    } 
                    else
                    {
                        // 既存の接続の処理
                        char buffer[1024] = {0};
                        int valread = read(pollfds[i].fd, buffer, 1024);
                        if (valread > 0) 
                        {
                            std::cout << "Received from client " << pollfds[i].fd << ": " << buffer << std::endl;
                            send(pollfds[i].fd, buffer, valread, 0);
                        }
                    }
                }
            }
        }
    }
    close(server_fd);
    
    return 0;
}
