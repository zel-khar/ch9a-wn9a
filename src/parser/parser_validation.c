#include "minishell.h"

static void syntax_err_msg(int type)
{
    const char *tok;

    if (type == TOKEN_REDIR_IN)
        tok = "<";
    else if (type == TOKEN_REDIR_OUT)
        tok = ">";
    else if (type == TOKEN_HEREDOC)
        tok = "<<";
    else if (type == TOKEN_APPEND)
        tok = ">>";
    else
        tok = "newline";
    dprintf(2, "minishell: syntax error near unexpected token `%s'\n", tok);
}

int validate_syntax(t_token *tok)
{
    if (!tok)
        return (1);
    /* A pipeline cannot start with a pipe token */
    if (tok->type == TOKEN_PIPE)
        return (dprintf(2,
                    "minishell: syntax error near unexpected token `|'\n"),
                0);
    while (tok)
    {
        if (tok->type == TOKEN_PIPE
            && (!tok->next || tok->next->type == TOKEN_PIPE))
            return (dprintf(2,
                        "minishell: syntax error near unexpected token `|'\n"),
                    0);
        if (tok->type >= TOKEN_REDIR_IN && tok->type <= TOKEN_APPEND)
        {
            if (!tok->next || tok->next->type != TOKEN_WORD)
            {
                syntax_err_msg(tok->next ? tok->next->type : -1);
                return (0);
            }
        }
        tok = tok->next;
    }
    return (1);
}
