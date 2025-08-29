#include "minishell.h"

static t_env	*find_env(t_env *env, const char *key)
{
	while (env)
	{
		if (!ft_strcmp(env->key, key))
			return (env);
		env = env->next;
	}
	return (NULL);
}

static int	count_env(t_env *env)
{
    int c = 0;
    while (env)
        c++, env = env->next;
    return (c);
}

static void	sort_env_ptrs(t_env **arr, int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (ft_strcmp(arr[i]->key, arr[j]->key) > 0)
            {
                t_env *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
}

static void	display_exports(t_env *env)
{
	int   n = count_env(env);
	t_env **arr = (t_env **)ft_malloc(sizeof(t_env *) * (n > 0 ? n : 1));
	int   i = 0;

	while (env)
	{
		arr[i++] = env;
		env = env->next;
	}
	sort_env_ptrs(arr, n);
	for (i = 0; i < n; i++)
	{
		ft_putstr_fd("declare -x ", 1);
		ft_putstr_fd(arr[i]->key, 1);
		if (arr[i]->value)
		{
			ft_putstr_fd("=\"", 1);
			ft_putstr_fd(arr[i]->value, 1);
			ft_putchar_fd('\"', 1);
		}
		ft_putchar_fd('\n', 1);
	}
}

static void	set_or_add_env(t_env **env, const char *key, const char *val, int append)
{
	t_env	*var;

    var = find_env(*env, key);
    if (var)
    {
        if (append && val)
        {
            if (var->value)
            {
                char *joined = ft_strjoin(var->value, val);
                var->value = joined;
            }
            else
                var->value = ft_strdup(val);
        }
        else
        {
            if (val)
                var->value = ft_strdup(val);
            else
                var->value = NULL;
        }
    }
    else
    {
        var = (t_env *)ft_malloc(sizeof(t_env));
        if (!var)
            return ;
        var->key = ft_strdup(key);
        if (val)
            var->value = ft_strdup(val);
        else
            var->value = NULL;
        var->next = *env;
        *env = var;
    }
}

static int	export_one(const char *arg, t_env **env)
{
    char    *eq;
    char    *key;
    int      append;

    {
        int k = 0;
        eq = NULL;
        while (arg[k])
        {
            if (arg[k] == '=')
            { eq = (char *)(arg + k); break; }
            k++;
        }
    }
    append = 0;
    if (eq && eq > arg && *(eq - 1) == '+')
    {
        append = 1;
        key = ft_strndup(arg, (eq - arg) - 1);
    }
    else if (eq)
        key = ft_strndup(arg, eq - arg);
    else
        key = ft_strdup(arg);
    if (!key)
        return (1);
    if (!is_valid_var_name(key))
    {
        ft_putstr_fd("minishell: export: '", 2);
        ft_putstr_fd(arg, 2);
        ft_putendl_fd("': not a valid identifier", 2);
        return (1);
    }
    if (eq)
        set_or_add_env(env, key, eq + 1, append);
    else
        set_or_add_env(env, key, NULL, 0);
    return (0);
}

int	builtin_export(char **argv, t_env **env)
{
    int status;
    int i;

    if (!argv[1])
        return (display_exports(*env), 0);
    status = 0;
    i = 1;
    while (argv[i])
    {
        status |= export_one(argv[i], env);
        i++;
    }
    return (status);
}
