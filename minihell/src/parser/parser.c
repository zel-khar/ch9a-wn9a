

#include "minishell.h"

/*
 * Count argv entries for a single command segment (until a pipe or end),
 * ignoring redirection targets. This allows arguments to appear before or
 * after redirections, like in: echo a > out b
 */
static int	count_cmd_args(t_token *tok)
{
	int	count;

	count = 0;
	while (tok && tok->type != TOKEN_PIPE)
	{
		if (is_redir(tok->type))
		{
			/* skip redirection operator and its target */
			tok = tok->next;
			if (tok)
				tok = tok->next;
			continue ;
		}
		if (tok->type == TOKEN_WORD)
			count++;
		tok = tok->next;
	}
	return (count);
}

static t_cmd	*append_new_cmd(t_cmd **head, t_cmd *cur)
{
	t_cmd	*tmp;

	if (cur)
		return (cur);
	cur = cmd_new();
	if (!*head)
		*head = cur;
	else
	{
		tmp = *head;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = cur;
	}
	return (cur);
}
/*
static int	set_redir(t_cmd *cmd, t_token *tok)
{
	if (!tok || !tok->next || !tok->next->value)
		return (0);
	if (tok->type == 2)
		cmd->infile = strdup(tok->next->value);
	if (tok->type == 3)
		cmd->outfile = strdup(tok->next->value);
	if (tok->type == 4) // HEREDOC
	{
		if (heredoc(tok->next->value) == -1)
			return (0);
	}
	if (tok->type == 5)
	{
		cmd->outfile = strdup(tok->next->value);
		cmd->append = 1;
	}
	return (0);
}*/
static int	set_redir(t_cmd *cmd, t_token *tok)
{
	if (!tok || !tok->next || !tok->next->value)
		return (0);
	if (tok->type == 2)
	{
		cmd->infile = ft_strdup(tok->next->value);
		add_redir(cmd, tok->type, tok->next->value);
	}
	if (tok->type == 3)
	{
		cmd->outfile = ft_strdup(tok->next->value);
		cmd->append = 0;
		add_redir(cmd, tok->type, tok->next->value);
	}
	if (tok->type == 4) // HEREDOC - Store delimiter, don't execute yet
	{
		cmd->heredoc_delimiter = ft_strdup(tok->next->value);
                cmd->heredoc_quoted = tok->next->quoted;
	}
	if (tok->type == 5)
	{
		cmd->outfile = ft_strdup(tok->next->value);
		cmd->append = 1;
		add_redir(cmd, tok->type, tok->next->value);
	}
	return (0);
}

/*
 * Fill argv for one command by scanning tokens until a pipe or end.
 * Redirections are applied to the command, and their targets are not
 * added to argv. Arguments can appear anywhere relative to redirections.
 */
static int	fill_argv_from_tokens(t_cmd *cur, t_token **tok)
{
	int		argc;
	int		i;
	t_token	*scan;

	argc = count_cmd_args(*tok);
	if (argc < 0)
		return (-1);
	if (argc > 0)
	{
		cur->argv = (char **)ft_malloc(sizeof(char *) * (argc + 1));
		if (!cur->argv)
			return (-1);
	}
	i = 0;
	scan = *tok;
	while (scan && scan->type != TOKEN_PIPE)
	{
		if (is_redir(scan->type))
		{
			set_redir(cur, scan);
			/* skip operator and its target */
			scan = scan->next;
			if (scan)
				scan = scan->next;
			continue ;
		}
		if (scan->type == TOKEN_WORD)
		{
			if (argc > 0)
				cur->argv[i++] = ft_strdup(scan->value);
		}
		scan = scan->next;
	}
	if (argc > 0)
		cur->argv[i] = NULL;
	/* Move caller's token pointer to the next segment (after pipe if present) */
	*tok = scan;
	return (0);
}

t_cmd	*parse_commands(t_token *tok, int *err)
{
	t_cmd	*head;
	t_cmd	*cur;

	head = NULL;
	cur = NULL;
	while (tok)
	{
		/* Start a new command segment */
		cur = append_new_cmd(&head, cur);
		if (!cur)
			return (NULL);
		if (fill_argv_from_tokens(cur, &tok) == -1)
			return (*err = 1, NULL);
		/* If current position is a pipe, advance to next command */
		if (tok && tok->type == TOKEN_PIPE)
		{
			cur->next = NULL;
			cur = NULL;
			tok = tok->next;
		}
		else
		{
			/* End of tokens for this command */
			break ;
		}
	}
	return (head);
}
