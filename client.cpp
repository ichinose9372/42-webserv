#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
	// ソケットの作成
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1) {
		std::cerr << "Failed to create socket." << std::endl;
		return 1;
	}

	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	// TODO:inet_addr()について調べる
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // サーバーのIPアドレス
	serverAddress.sin_port = htons(8080); // サーバーのポート番号

	// サーバーに接続
	if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		std::cerr << "Failed to connect." << std::endl;
		close(clientSocket);
		return 1;
	}

	// HTTP GETリクエストの作成
	const char* httpRequest = "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nConnection: close\r\n\r\n";

	// リクエストの送信
	send(clientSocket, httpRequest, strlen(httpRequest), 0);

	char buffer[1024];
	int bytesRead;

	// サーバーからの応答を受信し、表示
	while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
		buffer[bytesRead] = '\0'; // 終端文字をセット
		std::cout << "サーバーからの応答を受信し、表示する" << std::endl;
		std::cout << buffer;
	}

	close(clientSocket);

	return 0;
}
