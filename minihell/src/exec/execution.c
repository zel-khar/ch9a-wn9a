
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
    struct stat st;

    i = 0;
    while (paths && paths[i])
    {
        /* Empty PATH entry means current directory */
        if (paths[i][0] == '\0')
            dir = ft_strjoin(".", "/");
        else
            dir = ft_strjoin(paths[i], "/");
        full = ft_strjoin(dir, cmd);
        if (stat(full, &st) == 0 && S_ISDIR(st.st_mode))
        {
            i++;
            continue;
        }
        if (access(full, X_OK) == 0)
            return (full);
        i++;
    }
    return (NULL);
}

/* simple env lookup from our linked list */
static const char *env_get(t_env *env, const char *key)
{
    while (env)
    {
        if (!ft_strcmp(env->key, key))
            return (env->value ? env->value : "");
        env = env->next;
    }
    return NULL;
}

static char    **split_path_preserve_empty(const char *path)
{
    int count = 1;
    int i = 0;
    char **arr;
    int idx = 0;
    int start = 0;

    if (!path)
        path = "."; /* treat unset PATH as current directory */
    while (path[i])
        if (path[i++] == ':')
            count++;
    arr = (char **)ft_malloc(sizeof(char *) * (count + 1));
    if (!arr)
        return NULL;
    i = 0;
    while (1)
    {
        if (path[i] == ':' || path[i] == '\0')
        {
            if (i - start > 0)
                arr[idx++] = ft_strndup(path + start, i - start);
            else
                arr[idx++] = ft_strdup(""); /* empty entry */
            if (path[i] == '\0')
                break ;
            start = i + 1;
        }
        i++;
    }
    arr[idx] = NULL;
    return arr;
}

static char	*find_path(char *cmd, t_env *env)
{
    const char  *path;
    char        **paths;
    char        *full;

    if (!cmd || cmd[0] == '\0')
        return (ft_strdup(cmd));
    /* Special-case '.' and '..' when not builtins: do not resolve via PATH */
    if (!ft_strcmp(cmd, ".") || !ft_strcmp(cmd, ".."))
        return (NULL);
    {
        int idx = 0;
        while (cmd[idx] && cmd[idx] != '/')
            idx++;
        if (cmd[idx] == '/')
            return (ft_strdup(cmd));
    }
    path = env_get(env, "PATH");
    /* If PATH unset or empty, search current directory */
    if (!path || path[0] == '\0')
        paths = split_path_preserve_empty(".");
    else
        paths = split_path_preserve_empty(path);
    full = search_in_paths(paths, cmd);
    return (full);
}

int	exec_external(t_cmd *cmd, t_env *env, char **envp)
{
    char	*path;
    struct stat st;

    /* Empty command name => command not found (bash returns 127) */
    if (!cmd || !cmd->argv || !cmd->argv[0] || cmd->argv[0][0] == '\0')
    {
        char *name = (cmd && cmd->argv && cmd->argv[0]) ? cmd->argv[0] : "";
        write(2, "minishell: ", 11);
        write(2, name, ft_strlen(name));
        write(2, ": command not found\n",
            ft_strlen(": command not found\n"));
        return (127);
    }
    path = find_path(cmd->argv[0], env);
    if (!path)
    {
        write(2, "minishell: ", 11);
        write(2, cmd->argv[0], ft_strlen(cmd->argv[0]));
        write(2, ": command not found\n",
            ft_strlen(": command not found\n"));
        return (127);
    }
    /* With an explicit path (or resolved one), provide clearer diagnostics */
    if (stat(path, &st) == 0)
    {
        if (S_ISDIR(st.st_mode))
        {
            write(2, "minishell: ", 11);
            write(2, path, ft_strlen(path));
            write(2, ": Is a directory\n",
                ft_strlen(": Is a directory\n"));
            return (126);
        }
        if (access(path, X_OK) != 0)
        {
            write(2, "minishell: ", 11);
            write(2, path, ft_strlen(path));
            write(2, ": Permission denied\n",
                ft_strlen(": Permission denied\n"));
            return (126);
        }
    }
    execve(path, cmd->argv, envp);
    {
        int err = errno;
        perror("execve");
        return (err == ENOENT ? 127 : 126);
    }
}
