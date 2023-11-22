#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    const char* server_ip = "127.0.0.1"; // サーバーのIPアドレス
    int port = 8081;

    // ソケットの作成
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // IPv4アドレスを設定
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        return -1;
    }

    // サーバーに接続
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    // データの送受信（例えば、サーバーにメッセージを送信）
    while (true)
    {
        std::string message;
        std::cout << "Input message: ";
        std::cin >> message;
        send(sock, message.c_str(), message.size(), 0);

        if (message[0] == 'q')
        {
            break;
        }
        char buffer[1024] = {0};
        int valread = read(sock, buffer, 1024);
        std::cout << "Server reply: " << buffer << std::endl;
    }
    // ソケットを閉じる
    close(sock);

    return 0;
}
