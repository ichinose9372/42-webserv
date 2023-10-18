// https://daeudaeu.com/socket/
// 上記の記事でソケット通信のプログラムを組む

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080
#define BUF_SIZE 1024

int tranfer(int); 

int transfer(int sock)
{
    char send_buf[BUF_SIZE];
    char recv_buf;
    int send_size;
    int recv_size;

    while(1)
    {

        /* サーバーに送る文字列を取得 */
        printf("Input Message...\n");
        scanf("%s", send_buf);

        /* 文字列を送信 */
        send_size = send(sock, send_buf, strlen(send_buf) + 1, 0);

        if (send_size == -1)
        {
            printf("send error\n");
            break;
        }

        /* サーバーからの応答を受信　*/
        recv_size = recv(sock, &recv_buf, 1, 0);
        if (recv_size == -1)
        {
            printf("recv error\n");
            break;
        }
        if (recv_size == 0)
        {
            /* 受信サイズが0の場合は相手が接続閉じていると判断 */
            printf("connection ended\n");
            break;
        }

        /* 応答が0の場合は、データ送信終了*/
        if (recv_buf == 0)
        {
            printf("Finish connection\n");
            break;
        }
    }
    return 0;
}


int main(void)
{
    int sock;
    struct sockaddr_in addr;

    /* ソケットを作成 */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("socket error\n");
        return -1;
    }

    /* 構造体を全て0にセット */
    memset(&addr, 0, sizeof(struct sockaddr_in));

    /* サーバーのIPアドレスとポートの情報を設定 */
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    /* サーバーに接続要求送信 */
    printf("Start connect...\n");
    if (connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) {
        printf("connect error\n");
        close(sock);
        return -1;
    }
    printf("Finish connect!\n");

    /* 接続済のソケットでデータのやり取り */
    transfer(sock);

    /* ソケット通信をクローズ */
    close(sock);

    return 0;
}
