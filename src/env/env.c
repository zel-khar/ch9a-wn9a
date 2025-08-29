#include "minishell.h"
#include <ctype.h>

static t_env *make_env_node(const char *entry)
{
    t_env *new;
    char  *eq;

    new = (t_env *)ft_malloc(sizeof(t_env));
    if (!new)
        return (NULL);
    eq = strchr(entry, '=');
    if (eq)
    {
        new->key = ft_strdup(entry);
        *(new->key + (eq - entry)) = '\0';
        new->value = ft_strdup(eq + 1);
    }
    else
    {
        new->key = ft_strdup(entry);
        new->value = NULL;
    }
    new->next = NULL;
    return (new);
}

/* Copy environment to linked list */
t_env	*copy_env(char **envp)
{
    t_env *head;
    t_env *prev;
    int    i;

    head = NULL;
    prev = NULL;
    i = 0;
    while (envp[i])
    {
        t_env *node = make_env_node(envp[i]);
        if (!node)
            return (NULL);
        if (prev)
            prev->next = node;
        else
            head = node;
        prev = node;
        i++;
    }
    return (head);
}

/* Check if a variable name is valid */
int	is_valid_var_name(const char *name)
{
	if (!name || !*name || isdigit(*name))
		return (0);
	while (*name)
	{
		if (!isalnum(*name) && *name != '_')
			return (0);
		name++;
	}
	return (1);
}

static void put_env_line(const char *k, const char *v)
{
    char *line;
    char *full;

    line = ft_strjoin(k, "=");
    if (!line)
        return ;
    full = ft_strjoin(line, v);
    if (!full)
        return ;
    write(1, full, ft_strlen(full));
    write(1, "\n", 1);
}

void	display_env(t_env *env)
{
    t_env *tmp;

    tmp = env;
    while (tmp)
    {
        if (tmp->value)
            put_env_line(tmp->key, tmp->value);
        tmp = tmp->next;
    }
}
