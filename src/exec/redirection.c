#include "minishell.h"

static void heredoc_child_loop(int wfd, const char *delim, int quoted,
                               t_shell *shell)
{
    char *line;

    setup_heredoc_signals();
    while (1)
    {
        if (shell->interactive)
        {
            line = readline("> ");
        }
        else if (!strcmp(line, delim))
        {
            free(line);
            close(wfd); 
            exit(0);
        }
        else
        {
            int saved_stderr = dup(STDERR_FILENO);
            int devnull = open("/tmp/filename", O_CREAT | O_WRONLY | O_TRUNC, 0600);
            if (devnull != -1)
            {
                dup2(devnull, STDERR_FILENO); 
                close(devnull);
            }
            line = readline(NULL);
            if (saved_stderr != -1)
                dup2(saved_stderr, STDERR_FILENO), close(saved_stderr);
        }
        if (!line)
        {
            /* EOF: warn like bash about unfinished here-document */
            dprintf(2,
                "minishell: warning: here-document at line %d delimited by end-of-file (wanted `%s`)\n",
                shell->line_no, delim);
            close(wfd);
            exit(0);
        }
        if (!quoted)
        {
            char *exp = expand_line(line, shell);
            free(line);
            if (!exp)
                close(wfd), exit(1);
            if (!strcmp(exp, delim))
                close(wfd), exit(0);
            line = exp;
        }
        write(wfd, line, strlen(line));
        write(wfd, "\n", 1);
        free(line);
    }
}

static int heredoc_parent_setup(int rfd, pid_t pid)
{
    int status;

    signal(SIGINT, SIG_IGN);
    waitpid(pid, &status, 0);
    setup_signals();
    if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
        return (close(rfd), -1);
    if (dup2(rfd, STDIN_FILENO) == -1)
        return (perror("dup2"), close(rfd), -1);
    close(rfd);
    return (0);
}

int     handle_heredoc_execution(const char *delimiter, int quoted, t_shell *shell)
{
    int     pipe_fd[2];
    pid_t   pid;

    if (pipe(pipe_fd) == -1)
        return (perror("pipe"), -1);
    pid = fork();
    if (pid == 0)
        close(pipe_fd[0]), heredoc_child_loop(pipe_fd[1], delimiter, quoted, shell);
    else if (pid > 0)
    {
        close(pipe_fd[1]);
        return (heredoc_parent_setup(pipe_fd[0], pid));
    }
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    return (perror("fork"), -1);
}

int     redirect_input(const char *file)
{
    int fd;

    if (!file)
        return (-1);
    fd = open(file, O_RDONLY);
    if (fd == -1)
        return (perror(file), -1);
    if (dup2(fd, 0) == -1)
        return (perror("dup2"), close(fd), -1);
    close(fd);
    return (0);
}

int     redirect_output(const char *file, int append)
{
    int fd;
    int flags;

    if (!file)
        return (-1);
    flags = O_WRONLY | O_CREAT;
    if (append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;
    fd = open(file, flags, 0644);
    if (fd == -1)
        return (perror(file), -1);
    if (dup2(fd, 1) == -1)
        return (perror("dup2"), close(fd), -1);
    close(fd);
    return (0);
}
