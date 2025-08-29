#include "../inc/minishell.h"

static char	*get_input(t_shell *shell, char *prompt)
{
	char	*input;
	int		sstderr;
	int		devnull;
	if (shell->interactive)
		return (readline(prompt));
	sstderr = dup(STDERR_FILENO);
	devnull = open("/dev/null", O_WRONLY);
	dup2(devnull, STDERR_FILENO);
	close(devnull);
	input = readline(NULL);
	dup2(sstderr, STDERR_FILENO);
	close(sstderr);
	return (input);
}


static void	init_shell(t_shell *shell, char **envp)
{
	shell->envp = envp;
	shell->exit_status = 0;
	shell->env_list = copy_env(envp);
	rl_outstream = stderr;
	shell->interactive = isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
	if (!shell->interactive)
		rl_inhibit_completion = 1;
	shell->line_no = 1;
	setup_signals();
}

static void	run_loop(t_shell *shell)
{
	char	*line;
	int		err;

	while (1)
	{
		signal(SIGQUIT, SIG_IGN);
		line = get_input(shell, "minishell$ "); // echo $?
		/* If user sent SIGINT at prompt, reflect status like bash (130) */
		if (g_signal == SIGINT)
		{
			shell->exit_status = 130;
			g_signal = 0;
		}
		if (!line)
			break ;
		if (*line && shell->interactive)
			add_history(line);
		shell->tokens = tokenize(line, shell, &err);
		free(line);
                if (!err && !validate_syntax(shell->tokens))
                {
                        shell->exit_status = 2;
                        err = 1;
                }
		if (!err)
		{
			shell->cmds = parse_commands(shell->tokens, &err);
			if (!err)
				shell->exit_status = execute_pipeline(shell->cmds,
						&shell->env_list, shell->envp, shell);
		}
		if (err)
			shell->exit_status = 2;
		shell->tokens = NULL;
		shell->cmds = NULL;
		if (err && !shell->interactive)
			break ;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	init_shell(&shell, envp);
	run_loop(&shell);
	rl_clear_history();
	ft_cleanup(NULL);
	return (shell.exit_status);
}
