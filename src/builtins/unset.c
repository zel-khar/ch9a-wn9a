

#include "minishell.h"

static void	unset_one(const char *name, t_env **env)
{
    t_env *cur;
    t_env *prev;

    cur = *env;
    prev = NULL;
    while (cur)
    {
        if (!strcmp(cur->key, name))
        {
            if (prev)
                prev->next = cur->next;
            else
                *env = cur->next;
            /* With GC, we don't free here; memory is released on ft_cleanup */
            return ;
        }
        prev = cur;
        cur = cur->next;
    }
}

int	builtin_unset(char **argv, t_env **env)
{
    int i;

    i = 1;
    while (argv[i])
        unset_one(argv[i++], env);
    return (0);
}
