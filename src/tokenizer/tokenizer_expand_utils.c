#include "minishell.h"

/* forward decl from tokenizer_expand.c */
char		*tok_handle_env(const char *str, int *i, t_env *env,
				t_shell *shell);

char	*tok_str_append_char(char *acc, char c)
{
	char	buf[2];

	buf[0] = c;
	buf[1] = '\0';
	return (ft_strjoin(acc, buf));
}

static int	append_part_inner(char **res, char *part)
{
	*res = ft_strjoin(*res, part);
	if (*res)
		return (0);
	else
		return (-1);
}

char	*tok_expand_inner(const char *s, t_env *env, t_shell *shell)
{
	int		k;
	char	*res;
	char	*part;

    res = ft_strdup("");
    if (!res)
    {
        return (NULL);
    }
    k = 0;
	while (s[k])
	{
		if (s[k] == '$')
		{
            part = tok_handle_env(s, &k, env, shell);
            if (!part)
                return (NULL);
			if (append_part_inner(&res, part) == -1)
				return (NULL);
		}
		else if (!(res = tok_str_append_char(res, s[k++])))
			return (NULL);
	}
	return (res);
}

char	*tok_env_value_for_key(t_env *env, const char *key)
{
	while (env)
	{
		if (!ft_strcmp(env->key, key))
		{
			if (env->value)
				return (ft_strdup(env->value));
			else
				return (ft_strdup(""));
		}
		env = env->next;
	}
	return (ft_strdup(""));
}
