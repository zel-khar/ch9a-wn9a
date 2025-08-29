
#include "minishell.h"
/*
static char	*find_path(char *cmd, t_env *env)
{
	(void)env;  // Unused for now, will be used later for PATH lookup 
	char	*path;
	char	*dir;
	char	*full;
	char	**paths;
	int		i;

	if (!cmd || strchr(cmd, '/'))
		return (strdup(cmd));
	path = getenv("PATH");
	if (!path)
		return (NULL);
	paths = ft_split(path, ':');
	i = 0;
	while (paths && paths[i])
	{
		dir = ft_strjoin(paths[i], "/");
		full = ft_strjoin(dir, cmd);
		free(dir);
		if (access(full, X_OK) == 0)
		{
			ft_free_split(paths);
			return (full);
		}
		free(full);
		i++;
	}
	ft_free_split(paths);
	return (NULL);
}

int	exec_external(t_cmd *cmd, t_env *env, char **envp)
{
	char	*path;

	path = find_path(cmd->argv[0], env);
	if (!path)
	{
		printf("minishell: %s: command not found\n", cmd->argv[0]);
		return (127);
	}
	execve(path, cmd->argv, envp);
	perror("execve");
	free(path);
	return (126);
}*/

static char	*search_in_paths(char **paths, const char *cmd)
{
    char *dir;
    char *full;
    int   i;

    i = 0;
    while (paths && paths[i])
    {
        dir = ft_strjoin(paths[i], "/");
        full = ft_strjoin(dir, cmd);
        if (access(full, X_OK) == 0)
            return (full);
        i++;
    }
    return (NULL);
}

static char	*find_path(char *cmd, t_env *env)
{
    char    *path;
    char    **paths;
    char    *full;

    (void)env;
    if (!cmd || cmd[0] == '\0' || strchr(cmd, '/'))
        return (strdup(cmd));
    path = getenv("PATH");
    if (!path)
        return (NULL);
    paths = ft_split(path, ':');
    full = search_in_paths(paths, cmd);
    return (full);
}

int	exec_external(t_cmd *cmd, t_env *env, char **envp)
{
	char	*path;

    /* Empty command name => command not found (bash returns 127) */
    if (!cmd || !cmd->argv || !cmd->argv[0] || cmd->argv[0][0] == '\0')
    {
        fprintf(stderr, "minishell: %s: command not found\n",
                (cmd && cmd->argv && cmd->argv[0]) ? cmd->argv[0] : "");
        return (127);
    }
    path = find_path(cmd->argv[0], env);
    if (!path)
    {
        fprintf(stderr, "minishell: %s: command not found\n", cmd->argv[0]);
        return (127);
    }
    execve(path, cmd->argv, envp);
    perror("execve");
    return (126);
}
