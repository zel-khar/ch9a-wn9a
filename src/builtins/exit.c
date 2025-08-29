
#include "minishell.h"

static int	skip_ws_and_sign(const char *s, int *i)
{
	while (s[*i] && (s[*i] == ' ' || s[*i] == '\t'))
		(*i)++;
	if (s[*i] == '-' || s[*i] == '+')
		(*i)++;
	return (!!s[*i]);
}

static int	check_digits_tail(const char *s, int i)
{
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
		{
			while (s[i] && (s[i] == ' ' || s[i] == '\t'))
				i++;
			return (!s[i]);
		}
		i++;
	}
	return (1);
}

static int	is_numeric(const char *str)
{
	int i;

	i = 0;
	if (!skip_ws_and_sign(str, &i) || str[i] < '0' || str[i] > '9')
		return (0);
	return (check_digits_tail(str, i + 1));
}

int	builtin_exit(char **argv, t_shell *shell)
{
	int	status;

	ft_putendl_fd("exit", 2);
    if (!argv[1])
    {
            shell->tokens = NULL;
            shell->cmds = NULL;
            ft_cleanup(NULL);
            exit(shell->exit_status);
    }
    if (!is_numeric(argv[1]))
    {
            ft_putstr_fd("minishell: exit: ", 2);
            ft_putstr_fd(argv[1], 2);
            ft_putendl_fd(": numeric argument required", 2);
            shell->tokens = NULL;
            shell->cmds = NULL;
            ft_cleanup(NULL);
            exit(2);
    }
	if (argv[2])
	{
			ft_putendl_fd("minishell: exit: too many arguments", 2);
			return (1);
	}
	status = ft_atoi(argv[1]);
    shell->tokens = NULL;
    shell->cmds = NULL;
	ft_cleanup(NULL);
	exit(status & 255);
}
