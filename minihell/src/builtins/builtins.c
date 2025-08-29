#include "minishell.h"

static int	is_n_option(char *arg)
{
	int	j;

	if (arg[0] != '-')
		return (0);
	if (!arg[1])
		return (0);
	j = 1;
	while (arg[j])
	{
		if (arg[j] != 'n')
			return (0);
		j++;
	}
	return (1);
}


int	builtin_echo(char **argv)
{
	int	i;
	int	nl;
	int	written;

	i = 1;
	nl = 1;
	written = 0;
	while (argv[i] && is_n_option(argv[i]))
	{
		nl = 0;
		i++;
	}
	while (argv[i])
	{
        if (written)
            write(1, " ", 1);
        write(1, argv[i], ft_strlen(argv[i]));
		written = 1;
		i++;
	}
	if (nl)
		write(1, "\n", 1);
	return (0);
}

int	builtin_pwd(void)
{
    char	cwd[4096];

    if (getcwd(cwd, 4096))
    {
        ft_putendl_fd(cwd, 1);
        return (0);
    }
	perror("pwd");
	return (1);
}

int	builtin_env(t_env *env)
{
	t_env	*tmp;

	tmp = env;
	while (tmp)
	{
		if (tmp->value)
		{
			char *line = ft_strjoin(tmp->key, "=");
			char *full = ft_strjoin(line, tmp->value);
			ft_putendl_fd(full, 1);
		}
		tmp = tmp->next;
	}
	return (0);
}

int	execute_builtin(t_cmd *cmd, t_env **env, t_shell *shell)
{
    if (!ft_strcmp(cmd->argv[0], "echo"))
        return (builtin_echo(cmd->argv));
    if (!ft_strcmp(cmd->argv[0], "cd"))
        return (builtin_cd(cmd->argv, env));
    if (!ft_strcmp(cmd->argv[0], "pwd"))
        return (builtin_pwd());
    if (!ft_strcmp(cmd->argv[0], "export"))
        return (builtin_export(cmd->argv, env));
    if (!ft_strcmp(cmd->argv[0], "unset"))
        return (builtin_unset(cmd->argv, env));
    if (!ft_strcmp(cmd->argv[0], "env"))
        return (builtin_env(*env));
    if (!ft_strcmp(cmd->argv[0], "exit"))
        return (builtin_exit(cmd->argv, shell));
    return (1);
}
