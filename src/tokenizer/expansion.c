
#include "minishell.h"

char	*expand_var(const char *str, int *i, t_env *env, int last_status)
{
	int		start;
	int		len;
	char	*key;
	t_env	*tmp;

	start = *i;
	(*i)++;
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_itoa(last_status));
	}
	while (str[*i] && (str[*i] == '_' || (str[*i] >= 'A' && str[*i] <= 'Z') || (str[*i] >= 'a' && str[*i] <= 'z') || (str[*i] >= '0' && str[*i] <= '9')))
		(*i)++;
	len = *i - start - 1;
	key = ft_strndup(str + start + 1, len);
	tmp = env;
    while (tmp)
    {
        if (!strcmp(tmp->key, key))
        {
            if (tmp->value)
                return (ft_strdup(tmp->value));
            else
                return (ft_strdup(""));
        }
        tmp = tmp->next;
    }
    return (ft_strdup(""));
}

static char *append_str(char *acc, const char *add)
{
    return (ft_strjoin(acc, add));
}

static char *append_char(char *acc, char c)
{
    char buf[2];

    buf[0] = c;
    buf[1] = '\0';
    return (append_str(acc, buf));
}

char    *expand_line(const char *line, t_shell *shell)
{
    int     i;
    char    *res;
    char    *part;

    i = 0;
    res = ft_strdup("");
    if (!res)
        return (NULL);
    while (line[i])
    {
        if (line[i] == '$')
        {
            part = expand_var(line, &i, shell->env_list, shell->exit_status);
            if (!part)
                return (NULL);
            res = append_str(res, part);
            if (!res)
                return (NULL);
        }
        else if (!(res = append_char(res, line[i++])))
            return (NULL);
    }
    return (res);
}
