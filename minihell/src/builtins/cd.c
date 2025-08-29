#include "minishell.h"


int	builtin_cd(char **argv, t_env **env)
{
    char    *path;
    char    old_pwd[4096];
    int     have_old;

    have_old = (getcwd(old_pwd, sizeof(old_pwd)) != NULL);

    /* No args - cd to HOME */
    if (!argv[1])
    {
        t_env *home = *env;
        while (home && ft_strcmp(home->key, "HOME") != 0)
            home = home->next;
        if (!home || !home->value)
            return (ft_putendl_fd("minishell: cd: HOME not set", 2), 1);
        path = home->value;
    }
    /* Too many args */
    else if (argv[2])
        return (ft_putendl_fd("minishell: cd: too many arguments", 2), 1);
    else
        path = argv[1];

    if (chdir(path) == -1)
    {
        /* Match bash-like terse error in tricky states (e.g. deleted cwd) */
        if (errno == ENOENT)
            ft_putendl_fd("minishell: cd: No such file or directory", 2);
        else if (errno == ENOTDIR)
            ft_putendl_fd("minishell: cd: Not a directory", 2);
        else if (errno == EACCES)
            ft_putendl_fd("minishell: cd: Permission denied", 2);
        else
        {
            ft_putstr_fd("minishell: cd: ", 2);
            ft_putendl_fd("error", 2);
        }
        return (1);
    }

    /* Update PWD and OLDPWD if resolvable */
    {
        char new_pwd[4096];
        if (getcwd(new_pwd, sizeof(new_pwd)) != NULL)
        {
            t_env *var = *env;
            while (var)
            {
                if (have_old && ft_strcmp(var->key, "OLDPWD") == 0)
                    var->value = ft_strdup(old_pwd);
                else if (ft_strcmp(var->key, "PWD") == 0)
                    var->value = ft_strdup(new_pwd);
                var = var->next;
            }
        }
    }
    return (0);
}
