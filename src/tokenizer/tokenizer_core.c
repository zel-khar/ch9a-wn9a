#include "minishell.h"

static int	handle_operator_token(const char *str, int *i, t_token **head,
		int *heredoc_delim)
{
	int		type;
	char	*value;
	t_token	*new;

	type = tok_get_token_type(str, *i);
	if (type == TOKEN_HEREDOC || type == TOKEN_APPEND)
		value = ft_strndup(str + (*i), 2), *i += 2;
	else
		value = ft_strndup(str + (*i)++, 1);
	new = tok_token_new(value, type, 0);
	if (!new)
	{
		return (*head = NULL, 0);
	}
	tok_token_add_back(head, new);
	if (type == TOKEN_HEREDOC)
		*heredoc_delim = 1;
	return (1);
}

static int	add_splitted_tokens(char *value, t_token **head)
{
	int		i;
	int		start;
	t_token	*new;
	char	*part;

	i = 0;
	while (value[i])
	{
		while (value[i] && (value[i] == ' ' || value[i] == '\t'))
			i++;
		if (!value[i])
			break ;
		start = i;
		while (value[i] && value[i] != ' ' && value[i] != '\t')
			i++;
		part = ft_strndup(value + start, i - start);
		if (!part)
			return (*head = NULL, 0);
		new = tok_token_new(part, TOKEN_WORD, 0);
		if (!new)
			return (*head = NULL, 0);
		tok_token_add_back(head, new);
	}
	return (1);
}

static int	handle_word_token(const char *str, int *i, t_tokbuild *tb)
{
	int			quoted;
	char		*value;
	t_token		*new;
	char		*expanded;
	t_tokctx	ctx;

	quoted = 0;
	ctx.env = tb->shell->env_list;
	ctx.shell = tb->shell;
	ctx.heredoc = *(tb->heredoc_delim);
	ctx.err = tb->err;
	ctx.res = NULL;
	ctx.quoted = &quoted;
	value = tok_expand_word(str, i, &ctx); // echo $? -> "0"
	if (*(tb->err) || !value)
		return (*(tb->head) = NULL, 0);
	if (*(tb->heredoc_delim) && !quoted)
	{
		expanded = expand_line(value, tb->shell);
		if (!expanded)
			return (*(tb->head) = NULL, 0);
		value = expanded;
	}
	if (!quoted && !*(tb->heredoc_delim))
	{
		if (!add_splitted_tokens(value, tb->head))
			return (0);
	}
	else
	{
		new = tok_token_new(value, TOKEN_WORD, quoted);
		if (!new)
			return (*(tb->head) = NULL, 0);
		tok_token_add_back(tb->head, new);
	}
	*(tb->heredoc_delim) = 0;
	return (1);
}

t_token	*tokenize(const char *str, t_shell *shell, int *err)
{
	t_token		*head;
	int			i;
	int			heredoc_delim;
	t_tokbuild	tb;

	head = NULL;
	i = 0;
	heredoc_delim = 0;
	*err = 0;
	tb.head = &head;
	tb.heredoc_delim = &heredoc_delim;
	tb.shell = shell;
	tb.err = err;
	while (str[i]) // echo $?
	{
		i = tok_skip_spaces(str, i);
		if (!str[i])
			break ;
		if (tok_is_operator(str[i]))
		{
			if (!handle_operator_token(str, &i, &head, &heredoc_delim))
				return (NULL);
		}
		else if (!handle_word_token(str, &i, &tb))
			return (NULL);
	}
	return (head);
}
