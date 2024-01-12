#include "Signal.hpp"

// デフォルト動作でプログラムを終了するシグナルを無視する
void ignore_sig(void)
{
	// signal(SIGTERM, SIG_IGN);
	// signal(SIGINT, SIG_IGN);
	// signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGALRM, SIG_IGN);
	signal(SIGUSR1, SIG_IGN);
	signal(SIGUSR2, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
}
