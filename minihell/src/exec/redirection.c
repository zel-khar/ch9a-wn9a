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
        else
        {
            int saved_stderr = dup(STDERR_FILENO);
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull != -1)
                dup2(devnull, STDERR_FILENO), close(devnull);
            line = readline(NULL);
            if (saved_stderr != -1)
                dup2(saved_stderr, STDERR_FILENO), close(saved_stderr);
        }
        if (!line)
        {
            /* EOF warning like bash */
            {
                char *ln = ft_itoa(shell->line_no);
                write(2, "minishell: warning: here-document at line ", 43);
                if (ln)
                    write(2, ln, ft_strlen(ln));
                write(2, " delimited by end-of-file (wanted `", 35);
                write(2, delim, ft_strlen(delim));
                write(2, "`)\n", 3);
            }
            close(wfd);
            exit(0);
        }
        if (!quoted)
        {
            char *exp = expand_line(line, shell);
            free(line);
            if (!exp)
                close(wfd), exit(1);
            if (!ft_strcmp(exp, delim))
                close(wfd), exit(0);
            line = exp;
        }
        else if (!ft_strcmp(line, delim))
            free(line), close(wfd), exit(0);
        write(wfd, line, ft_strlen(line));
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

/* Apply all redirections in left-to-right order:
 * - Open/validate all but the final input/output to trigger side-effects
 * - Then dup2 the final input/output using existing helpers
 */
int     apply_all_redirs(t_cmd *cmd)
{
    t_redir *it;

    it = cmd->redirs;
    while (it)
    {
        if (it->type == TOKEN_REDIR_IN)
        {
            if (redirect_input(it->file) == -1)
                return (-1);
        }
        else if (it->type == TOKEN_REDIR_OUT || it->type == TOKEN_APPEND)
        {
            if (redirect_output(it->file, it->append) == -1)
                return (-1);
        }
        it = it->next;
    }
    return (0);
}
