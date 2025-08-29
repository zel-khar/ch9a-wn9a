

#include "minishell.h"

static void setup_output_redir(t_cmd *cmd)
{
    int flags;
    int fd;

    if (!cmd->outfile)
        return ;
    flags = O_WRONLY | O_CREAT;
    if (cmd->append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;
    fd = open(cmd->outfile, flags, 0644);
    if (fd == -1)
        perror("open"), exit(1);
    dup2(fd, 1);
    close(fd);
}

int	child_process(t_cmd *cmd, t_env *env, char **envp, int in_fd)
{
    if (cmd->infile)
        in_fd = open(cmd->infile, O_RDONLY);
    setup_output_redir(cmd);
    if (in_fd != 0)
        dup2(in_fd, 0), close(in_fd);
    if (cmd->pipe_fd[1] != -1)
        dup2(cmd->pipe_fd[1], 1), close(cmd->pipe_fd[1]);
    return (exec_external(cmd, env, envp));
}

int	wait_for_children(int n, int *status)
{
    int ret;
    int i;

    i = 0;
    while (i++ < n)
        ret = wait(status);
    return (ret);
}
