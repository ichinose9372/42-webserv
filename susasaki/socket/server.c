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

int transfer(int);

int transfer(int sock)
{
    int recv_size;
    int send_size;
    char recv_buf[BUF_SIZE];
    char send_buf;

    while (1)
    {
        /* クライアントから文字列を受信*/
        recv_size = recv(sock, recv_buf, BUF_SIZE, 0);
        if (recv_size == -1)
        {
            printf("recv error\n");
            break;
        }
        if (recv_size == 0)
        {
            /* 受信サイズが0の場合はクライアントが接続を閉じていると判断*/
            printf("connection ended\n");
        }

        /* 受信した文字列を表示*/
        printf("受信した文字列は\"%s\"\n", recv_buf);

        /* 文字列が"finish"ならクライアントとの接続を終了"*/
        if (strcmp(recv_buf, "finish") == 0)
        {
            /* 接続終了を表す0を送信 */
            send_buf = 0;
            send_size = send(sock, &send_buf, 1, 0);
            if (send_size == -1)
            {
                printf("send error\n");
                break;
            }
            break;
        }
        else
        {
            /* "finish"以外の場合はクライアントとの接続を継続 */
            send_buf = 1;
            send_size = send(sock, &send_buf, 1, 0);
            if (send_size == -1)
            {
                printf("send error\n");
                break;
            }
        }
    }
    return 0;
}

int main(void)
{
    int w_addr;
    int c_sock;
    struct sockaddr_in a_addr;

    /* ソケットを作成 */
    w_addr = socket(AF_INET, SOCK_STREAM, 0);
    if (w_addr == -1)
    {
        printf("socket error\n");
        return -1;
    }

    /* 構造体を全て0にセット */
    memset(&a_addr, 0, sizeof(struct sockaddr_in));

    /* サーバーのIPアドレスとポートの情報を設定 */
    a_addr.sin_family = AF_INET;
    a_addr.sin_port = htons((unsigned short)SERVER_PORT);
    a_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    /* ソケットに情報を設定 */
    if (bind(w_addr, (const struct sockaddr *) &a_addr, sizeof(a_addr)) == -1)
    {
        printf("bind error\n");
        close(w_addr);
        return -1;
    }

    /* ソケットを接続待ちに設定 */
    if (listen(w_addr, 3) == -1)
    {
            printf("listen error\n");
            close(w_addr);
            return -1;
    }

    while (1)
    {
        /* 接続要求の受付 (接続要求くるまで待ち) */
        printf("Waiting connect...\n");
        c_sock = accept(w_addr, NULL, NULL);
        if (c_sock == -1)
        {
            printf("accept error\n");
            close(w_addr);
            return -1;
        }

        printf("Connected!!!\n");

        /* 接続済のソケットでデータのやり取り */
        transfer(c_sock);

        /* ソケット通信をクローズ */
        close(c_sock);
    }

    close(w_addr);

    return 0;
}