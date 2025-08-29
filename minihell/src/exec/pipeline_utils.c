#include "minishell.h"

int	prep_heredoc(t_cmd *cmd, t_shell *shell, int *saved_stdin)
{
    if (!cmd || !cmd->heredoc_delimiter)
        return (0);
    *saved_stdin = dup(STDIN_FILENO);
    shell->saved_stdin_fd = *saved_stdin;
    if (handle_heredoc_execution(cmd->heredoc_delimiter, cmd->heredoc_quoted,
            shell) == -1)
    {
        if (*saved_stdin != -1)
        {
            dup2(*saved_stdin, STDIN_FILENO);
            close(*saved_stdin);
        }
        shell->saved_stdin_fd = -1;
        return (1);
    }
    return (0);
}

int	spawn_child(int prev_fd, int *pipe_fd, t_cmd *cur, t_shell *shell)
{
	int	code;
	/* If heredoc is present for this command, keep STDIN as
		prepared by prep_heredoc; otherwise, take input from prev pipe. */
	if (prev_fd != -1)
	{
		if (cur->heredoc_delimiter)
			close(prev_fd);
		else
			dup2(prev_fd, 0), close(prev_fd);
	}
	if (cur->next)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], 1);
		close(pipe_fd[1]);
	}
        if (apply_all_redirs(cur) == -1)
                exit(1);
        if (!cur->argv || !cur->argv[0])
                code = 0;
        else if (is_builtin(cur->argv[0]))
                code = execute_builtin(cur, &shell->env_list, shell);
        else
                code = exec_external(cur, shell->env_list, shell->envp);
	/* Drop references; GC will cleanup */
	shell->cmds = NULL;
	shell->tokens = NULL;
	ft_cleanup(NULL);
	exit(code);
}
