
#include "minishell.h"

int	is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
	if (!ft_strcmp(cmd, "echo"))
		return (1);
	if (!ft_strcmp(cmd, "cd"))
		return (1);
	if (!ft_strcmp(cmd, "pwd"))
		return (1);
	if (!ft_strcmp(cmd, "export"))
		return (1);
	if (!ft_strcmp(cmd, "unset"))
		return (1);
	if (!ft_strcmp(cmd, "env"))
		return (1);
	if (!ft_strcmp(cmd, "exit"))
		return (1);
	return (0);
}

t_cmd	*cmd_new(void)
{
	t_cmd	*cmd;

	cmd = (t_cmd *)ft_malloc(sizeof(t_cmd));
	cmd->argv = NULL;
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->heredoc_delimiter = NULL;
	cmd->heredoc_quoted = 0;
	cmd->append = 0;
	cmd->pipe_fd[0] = -1;
	cmd->pipe_fd[1] = -1;
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

int	is_redir(int type)
{
	return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT
		|| type == TOKEN_HEREDOC || type == TOKEN_APPEND);
}

void	add_redir(t_cmd *cmd, int type, const char *file)
{
	t_redir *node;
	t_redir *it;

	if (!cmd || !file)
		return ;
	node = (t_redir *)ft_malloc(sizeof(t_redir));
	node->type = type;
	node->file = ft_strdup(file);
	node->append = (type == TOKEN_APPEND);
	node->next = NULL;
	if (!cmd->redirs)
		cmd->redirs = node;
	else
	{
		it = cmd->redirs;
		while (it->next)
			it = it->next;
		it->next = node;
	}
}
