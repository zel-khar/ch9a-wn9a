

#include "minishell.h"

static int	count_args(t_token *tok)
{
	int	count;
	count = 0;
	while (tok && tok->type == 0)
	{
		count++;
		tok = tok->next;
	}
	return (count);
}

static t_cmd *append_new_cmd(t_cmd **head, t_cmd *cur)
{
    t_cmd *tmp;

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
		cmd->infile = ft_strdup(tok->next->value);
	if (tok->type == 3)
		cmd->outfile = ft_strdup(tok->next->value);
	if (tok->type == 4) // HEREDOC - Store delimiter, don't execute yet
	{
		cmd->heredoc_delimiter = ft_strdup(tok->next->value);
                cmd->heredoc_quoted = tok->next->quoted;
	}
	if (tok->type == 5)
	{
		cmd->outfile = ft_strdup(tok->next->value);
		cmd->append = 1;
	}
	return (0);
}

static int fill_argv_from_tokens(t_cmd *cur, t_token **tok)
{
    int argc;
    int i;

    argc = count_args(*tok);
    if (argc <= 0)
        return (0);
    cur->argv = (char **)ft_malloc(sizeof(char *) * (argc + 1));
    if (!cur->argv)
        return (-1);
    i = 0;
    while (i < argc)
    {
        cur->argv[i++] = ft_strdup((*tok)->value);
        *tok = (*tok)->next;
    }
    cur->argv[i] = NULL;
    return (0);
}

t_cmd	*parse_commands(t_token *tok, int *err)
{
    t_cmd *head;
    t_cmd *cur;
    head = NULL;
    cur = NULL;
    while (tok)
    {
        cur = append_new_cmd(&head, cur);
        if (!cur)
            return (NULL);
        if (fill_argv_from_tokens(cur, &tok) == -1)
            return (*err = 1, NULL);
        if (tok && is_redir(tok->type))
        {
            set_redir(cur, tok);
            tok = tok->next;
            if (tok)
                tok = tok->next;
        }
        if (tok && tok->type == TOKEN_PIPE)
        {
            cur->next = NULL;
            cur = NULL;
            tok = tok->next;
        }
    }
    return (head);
}

