#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

int main(void) {
    int fd = 0; // 標準入力を監視するためのファイルディスクリプタ
    fd_set rfds; // 読み取りを監視するファイルディスクリプタセット
    struct timeval tv; // タイムアウト設定
    int retval; // selectの戻り値

    // メモリの割り当て
    char inputval[256]; // 入力バッファ

    FD_ZERO(&rfds); // ファイルディスクリプタセットの初期化
    FD_SET(fd, &rfds); // 標準入力をセットに追加

    // タイムアウト時間の設定（5.5秒）
    tv.tv_sec = 5;
    tv.tv_usec = 500000;

    retval = select(fd + 1, &rfds, NULL, NULL, &tv);

    if (retval < 0) {
        perror("select()"); // エラーの場合
    } else if (retval) {
        scanf("%255s", inputval); // 入力がある場合、それを読み取る
        if (FD_ISSET(0,&rfds))
        {
            printf("input: %s %d\n", inputval, retval);
        }
    } else {
        printf("timeout %d\n", retval); // タイムアウトの場合
    }

    return (EXIT_SUCCESS);
}
