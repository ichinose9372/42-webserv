#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Failed to create sockt." << std::endl;
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    if (bind(serverSocket,(struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Failed to bind." << std::endl;
        return 1;
    }

    if (listen(serverSocket, 10) == -1)
    {
        std::cerr << "Failed to listen." << std::endl;
        return 1;
    }

    std::cout << "Server is listenig on port 8080..." << std::endl;

    while (true)
    {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clinetSocket = accept(serverSocket, (struct sockaddr *)&clientAddress,
                                  &clientAddressSize);
        if (clinetSocket == -1)
        {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        char buffer[1024];
        int byteRead = recv(clinetSocket, buffer, sizeof(buffer), 0);

        std::string customMessage = "<html><body><h1>Hello World!</h1></body></html>";

        if (byteRead > 0)
        {
            // HTTP1.1 レスポンスとしてリクエストをそのまま返す
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" + customMessage;
            send(clinetSocket, response.c_str(), response.length(), 0);
        }

        close(clinetSocket);
    }
    close(serverSocket);

    return 0;
}