#include "minishell.h"


int	builtin_cd(char **argv, t_env **env)
{
	char *path;
	char old_pwd[4096];

	if (getcwd(old_pwd, sizeof(old_pwd)) == NULL)
	{
		perror("minishell: cd");
		return (1);
	}

	/* No args - cd to HOME */
	if (!argv[1])
	{
		t_env *home = *env;
		while (home && strcmp(home->key, "HOME") != 0)
			home = home->next;
		if (!home || !home->value)
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			return (1);
		}
		path = home->value;
	}
	/* Too many args */
	else if (argv[2])
	{
		ft_putendl_fd("minishell: cd: too many arguments", 2);
		return (1);
	}
	else
		path = argv[1];

	if (chdir(path) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": ", 2);
		perror("");
		return (1);
	}

	/* Update PWD and OLDPWD */
	char new_pwd[4096];
	if (getcwd(new_pwd, sizeof(new_pwd)) != NULL)
	{
		t_env *var = *env;
		while (var)
		{
                        if (strcmp(var->key, "OLDPWD") == 0)
                                var->value = ft_strdup(old_pwd);
                        else if (strcmp(var->key, "PWD") == 0)
                                var->value = ft_strdup(new_pwd);
			var = var->next;
		}
	}

	return (0);
}
