#include "minishell.h"

int			prep_heredoc(t_cmd *cmd, t_shell *shell, int *saved_stdin);

static int	run_single_builtin(t_cmd *cmd, t_env **env, t_shell *shell,
		int saved_stdin)
{
	int	saved_out;
	int	ret;

	saved_out = dup(STDOUT_FILENO);
	if ((cmd->outfile && redirect_output(cmd->outfile, cmd->append) == -1)
		|| (cmd->infile && redirect_input(cmd->infile) == -1))
	{
		if (saved_out != -1)
			dup2(saved_out, STDOUT_FILENO), close(saved_out);
		if (saved_stdin != -1)
			dup2(saved_stdin, STDIN_FILENO), close(saved_stdin);
		return (1);
	}
	ret = execute_builtin(cmd, env, shell);
	if (saved_out != -1)
		dup2(saved_out, STDOUT_FILENO), close(saved_out);
	if (saved_stdin != -1)
		dup2(saved_stdin, STDIN_FILENO), close(saved_stdin);
	return (ret);
}

static int	run_external_single(t_cmd *cmd, t_env **env, char **envp,
		int saved_stdin)
{
    pid_t	pid;
    int		status;

	pid = fork();
	if (pid == 0)
	{
		setup_signals();
		if (cmd->infile && redirect_input(cmd->infile) == -1)
			exit(1);
		if (cmd->outfile && redirect_output(cmd->outfile, cmd->append) == -1)
			exit(1);
		exit(exec_external(cmd, *env, envp));
	}
    if (pid > 0)
    {
        signal(SIGINT, SIG_IGN);
        /* Wait specifically for this child to avoid reaping others */
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
            if (saved_stdin != -1)
                dup2(saved_stdin, STDIN_FILENO), close(saved_stdin);
            setup_signals();
            return (1);
        }
        setup_signals();
        if (saved_stdin != -1)
            dup2(saved_stdin, STDIN_FILENO), close(saved_stdin);
        if (WIFSIGNALED(status))
            return (128 + WTERMSIG(status));
        return (WEXITSTATUS(status));
    }
	if (saved_stdin != -1)
		dup2(saved_stdin, STDIN_FILENO), close(saved_stdin);
	perror("fork");
	return (1);
}

int	execute_single_cmd(t_cmd *cmd, t_env **env, char **envp, t_shell *shell)
{
	int	saved_stdin;
	int	saved_stdout;
	int	ret;

	if (!cmd)
		return (0);
	saved_stdin = -1;
	if (prep_heredoc(cmd, shell, &saved_stdin))
		return (1);
	if (!cmd->argv || !cmd->argv[0])
	{
		saved_stdout = dup(STDOUT_FILENO);
		ret = 0;
		if (cmd->infile && redirect_input(cmd->infile) == -1)
			ret = 1;
		else if (cmd->outfile && redirect_output(cmd->outfile, cmd->append) ==
			-1)
			ret = 1;
		if (saved_stdout != -1)
			dup2(saved_stdout, STDOUT_FILENO), close(saved_stdout);
		if (saved_stdin != -1)
			dup2(saved_stdin, STDIN_FILENO), close(saved_stdin);
		return (ret);
	}
	if (is_builtin(cmd->argv[0]))
		return (run_single_builtin(cmd, env, shell, saved_stdin));
	return (run_external_single(cmd, env, envp, saved_stdin));
}

// int	execute_single_cmd(t_cmd *cmd, t_env **env, char **envp, t_shell *shell)
// {
// 	pid_t	pid;
// 	int		status;

// 	if (!cmd || !cmd->argv || !cmd->argv[0])
// 		return (0);
// 	if (is_builtin(cmd->argv[0]))
// 	{
// 		int saved_stdout = dup(STDOUT_FILENO);
// 		int saved_stdin = dup(STDIN_FILENO);
// 		int ret;

// 		if (cmd->outfile && redirect_output(cmd->outfile, cmd->append) == -1)
// 		{
// 			if (saved_stdout != -1)
// 				dup2(saved_stdout, STDOUT_FILENO);
// 			if (saved_stdin != -1)
// 				dup2(saved_stdin, STDIN_FILENO);
// 			return (1);
// 		}
// 		if (cmd->infile && redirect_input(cmd->infile) == -1)
// 		{
// 			if (saved_stdout != -1)
// 				dup2(saved_stdout, STDOUT_FILENO);
// 			if (saved_stdin != -1)
// 				dup2(saved_stdin, STDIN_FILENO);
// 			return (1);
// 		}
// 		ret = execute_builtin(cmd, env, shell);
// 		if (saved_stdout != -1)
// 		{
// 			dup2(saved_stdout, STDOUT_FILENO);
// 			close(saved_stdout);
// 		}
// 		if (saved_stdin != -1)
// 		{
// 			dup2(saved_stdin, STDIN_FILENO);
// 			close(saved_stdin);
// 		}
// 		return (ret);
// 	}
// 	pid = fork();
// 	if (pid == 0)
// 	{
// 		setup_signals();  // Setup signals for child
// 		if (cmd->infile && redirect_input(cmd->infile) == -1)
// 			exit(1);
// 		if (cmd->outfile && redirect_output(cmd->outfile, cmd->append) == -1)
// 			exit(1);
// 		exit(exec_external(cmd, *env, envp));
// 	}
// 	else if (pid > 0)
// 	{
// 		signal(SIGINT, SIG_IGN);  // Parent ignores Ctrl+C during wait
// 		wait(&status);
// 		setup_signals();  // Restore signals
// 		return (WEXITSTATUS(status));
// 	}
// 	perror("fork");
// 	return (1);
// }

static int	pipeline_loop(t_cmd *cmds, t_shell *shell)
{
	t_cmd	*cur;
	int		prev_fd;
	int		pipe_fd[2];
	int		status;
	pid_t	last_cmd;
	int		saved_stdin;

	prev_fd = -1;
	cur = cmds;
	while (cur)
	{
		saved_stdin = -1;
		if (prep_heredoc(cur, shell, &saved_stdin))
		{
			if (prev_fd != -1)
				close(prev_fd);
			return (1);
		}
		if (cur->next && pipe(pipe_fd) == -1)
		{
			if (prev_fd != -1)
				close(prev_fd);
			if (saved_stdin != -1)
				dup2(saved_stdin, STDIN_FILENO), close(saved_stdin);
			return (perror("pipe"), 1);
		}
    last_cmd = fork();
    if (last_cmd == 0)
        spawn_child(prev_fd, pipe_fd, cur, shell);
    /* parent restores its original stdin after forking */
    if (saved_stdin != -1)
        dup2(saved_stdin, STDIN_FILENO), close(saved_stdin);
		if (prev_fd != -1)
			close(prev_fd);
		if (cur->next)
		{
			close(pipe_fd[1]);
			prev_fd = pipe_fd[0];
		}
		cur = cur->next;
	}
	while (waitpid(last_cmd, &status, 0) > 0)
		;
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (128 + WTERMSIG(status));
}

int	execute_pipeline(t_cmd *cmds, t_env **env, char **envp, t_shell *shell)
{
	t_cmd	*cur;
	int		count;

	if (!cmds)
		return (0);
	cur = cmds;
	count = 0;
	while (cur)
	{
		count++;
		cur = cur->next;
	}
	if (count == 1)
		return (execute_single_cmd(cmds, env, envp, shell));
	return (pipeline_loop(cmds, shell));
}
