// デフォルト動作でプログラムを終了するシグナルを無視する
// SIGINT : ctrl+c
// SIGPIPE : 接続先がないパイプやソケットの書き込み
// SIGALRM : alarm(2)からのタイマー
// SIGTERM/SIGUSR1/SIGUSR2 : killコマンド

#include <signal.h>

void ignore_sig(void);
