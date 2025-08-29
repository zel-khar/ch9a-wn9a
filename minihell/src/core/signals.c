

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

static void	sigint_handler(int signo)
{
	g_signal = signo;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

static void	sigquit_handler(int signo)
{
	g_signal = signo;
	write(1, "Quit: 3\n", 8);
}

void	setup_heredoc_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
}

void	setup_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);

	sa.sa_handler = sigquit_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGQUIT, &sa, NULL);

	signal(SIGTSTP, SIG_IGN);
}
