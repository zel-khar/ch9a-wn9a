#include "minishell.h"

int tok_is_operator(char c)
{
    return (c == '|' || c == '<' || c == '>');
}

int tok_skip_spaces(const char *str, int i)
{
    while (str[i] && (str[i] == ' ' || str[i] == '\t'))
        i++;
    return (i);
}

int tok_get_token_type(const char *str, int i)
{
    if (str[i] == '|')
        return (TOKEN_PIPE);
    if (str[i] == '<' && str[i + 1] == '<')
        return (TOKEN_HEREDOC);
    if (str[i] == '>' && str[i + 1] == '>')
        return (TOKEN_APPEND);
    if (str[i] == '<')
        return (TOKEN_REDIR_IN);
    if (str[i] == '>')
        return (TOKEN_REDIR_OUT);
    return (TOKEN_WORD);
}

t_token *tok_token_new(char *value, int type, int quoted)
{
    t_token *token;

    token = (t_token *)ft_malloc(sizeof(t_token));
    token->value = value;
    token->type = type;
    token->quoted = quoted;
    token->next = NULL;
    return (token);
}

void tok_token_add_back(t_token **lst, t_token *new)
{
    t_token *tmp;

    if (!*lst)
    {
        *lst = new;
        return ;
    }
    tmp = *lst;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = new;
}
