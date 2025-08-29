#include "minishell.h"

/* helpers from utils */
char    *tok_str_append_char(char *acc, char c);
char    *tok_expand_inner(const char *s, t_env *env, t_shell *shell);
char    *tok_env_value_for_key(t_env *env, const char *key);

char *tok_handle_env(const char *str, int *i, t_env *env, t_shell *shell)
{
    int     start;
    char    *key;

    start = *i;
    (*i)++;
    if (str[*i] == '?')
        return ((*i)++, ft_itoa(shell->exit_status));
    if (!ft_isalpha(str[*i]) && str[*i] != '_')
        return (ft_strdup("$"));
    while (str[*i] && (str[*i] == '_' || ft_isalnum(str[*i])))
        (*i)++;
    key = ft_strndup(str + start + 1, *i - start - 1);
    if (!key)
        return (NULL);
    {
        char *val = tok_env_value_for_key(env, key);
        return (val);
    }
}

/* tok_expand_inner moved to tokenizer_expand_utils.c */

static char *tok_handle_quotes(const char *str, int *i, t_tokctx *ctx)
{
    char quote;
    int j;
    char *content;

    quote = str[*i];
    j = *i + 1;
    while (str[j] && str[j] != quote)
        j++;
    if (!str[j])
        return (*(ctx->err) = 1, NULL);
    content = ft_strndup(str + *i + 1, j - *i - 1);
    if (!content)
        return (NULL);
    if (quote == '"' && !ctx->heredoc)
    {
        char *expanded = tok_expand_inner(content, ctx->env, ctx->shell);
        content = expanded;
        if (!content)
            return (NULL);
    }
    *i = j + 1;
    return (content);
}

static int join_part(char **res, char *part)
{
    char *tmp;

    /* With GC-managed allocations, do not free here.
       Just join and update the accumulator; GC will collect later. */
    tmp = ft_strjoin(*res, part);
    *res = tmp;
    if (tmp)
        return (0);
    else
        return (-1);
}

static int process_word_step(const char *s, int *i, t_tokctx *ctx, char **res)
{
    char *part;

    if (s[*i] == '\'' || s[*i] == '"')
    {
        if (ctx->quoted)
            *(ctx->quoted) = 1;
        part = tok_handle_quotes(s, i, ctx);
        if (!part)
            return (-1);
        return (join_part(res, part));
    }
    if (s[*i] == '$' && !ctx->heredoc)
    {
        part = tok_handle_env(s, i, ctx->env, ctx->shell);
        if (!part)
            return (-1);
        return (join_part(res, part));
    }
    *res = tok_str_append_char(*res, s[*i]);
    (*i)++;
    if (*res)
        return (0);
    else
        return (-1);
}

char *tok_expand_word(const char *str, int *i, t_tokctx *ctx)
{
    char *res;

    res = ft_strdup("");
    if (!res)
        return (NULL);
    while (str[*i] && !(str[*i] == ' ' || str[*i] == '\t'
        || str[*i] == '|' || str[*i] == '<' || str[*i] == '>'))
    {
        if (process_word_step(str, i, ctx, &res) == -1)
            return (NULL);
    }
    return (res);
}
