#ifndef MINISHELL_H
# define MINISHELL_H

# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <signal.h>
# include <dirent.h>
# include <string.h>
# include <termios.h>
# include <errno.h>
# include <fcntl.h>
# include "../libft/libft.h"

/* Internal tokenizer sentinel for protected whitespace inside quotes */
#define TOK_WS_SENTINEL 31

/* Token types */
#define TOKEN_WORD 0
#define TOKEN_PIPE 1
#define TOKEN_REDIR_IN 2
#define TOKEN_REDIR_OUT 3
#define TOKEN_HEREDOC 4
#define TOKEN_APPEND 5

/* ************************************************************************** */
/*   STRUCTURES                                                              */
/* ************************************************************************** */

/* Token structure for parsing */
typedef struct s_token
{
    char		*value; /* Token string */
    int			type;  /* Token type (word, pipe, redir, etc) */
    int                 quoted; /* Was token enclosed in quotes */
    struct s_token	*next;
} t_token;

/* Command structure for execution */
typedef struct s_redir t_redir;
typedef struct s_cmd
{
    char		**argv; /* Command arguments */
    char		*infile; /* Input redirection file */
    char		*outfile; /* Output redirection file */
    char    *heredoc_delimiter; /* Heredoc delimiter */
    int                 heredoc_quoted; /* Was the delimiter quoted */
    int                 append; /* Append mode for output */
    int			pipe_fd[2]; /* Pipe file descriptors */
    t_redir         *redirs; /* Ordered redirections for side-effects */
    struct s_cmd	*next;
} t_cmd;

/* Ordered redirection operations for a command */
typedef struct s_redir
{
    int                 type;   /* TOKEN_REDIR_IN / TOKEN_REDIR_OUT / TOKEN_APPEND */
    char               *file;   /* Target filename */
    int                 append; /* For output: 1 if append (>>), 0 if truncate (>) */
    struct s_redir     *next;
}   t_redir;

/* Environment variable structure */
typedef struct s_env
{
    char		*key;
    char		*value;
    struct s_env	*next;
} t_env;

typedef struct s_malloc
{
    void                *content;
    struct s_malloc     *next;
}   t_malloc;

/* Shell state structure */
typedef struct s_shell
{
    char		*prompt; /* Current prompt string */
    t_env		*env_list; /* Linked list of env variables */
    char		**envp; /* Environment for execve */
    t_token		*tokens; /* Token list for current input */
    t_cmd		*cmds; /* Parsed commands for current input */
    int			exit_status; /* Last exit status */
    int         interactive; /* Interactive mode flag */
    int         line_no; /* Current input line number */
    int         saved_stdin_fd; /* safety: dup of stdin during heredoc */
} t_shell;

/* Token expansion context to keep function args <= 4 */
typedef struct s_tokctx {
    t_env   *env;
    t_shell *shell;
    int     heredoc;
    int     *err;
    char    **res;     /* optional accumulator pointer */
    int     *quoted;   /* optional quoted flag pointer */
} t_tokctx;

typedef struct s_tokbuild {
	t_token **head;
	int     *heredoc_delim;
	t_shell *shell;
	int     *err;
} t_tokbuild;

/* ************************************************************************** */
/*   MEMORY MANAGEMENT NOTES                                                  */
/*   - All data is allocated with ft_malloc and freed by ft_cleanup           */
/*   - To discard parsed structures, just set pointers to NULL                */
/* ************************************************************************** */

/* ************************************************************************** */
/*   FUNCTION PROTOTYPES                                                     */
/* ************************************************************************** */

/* Tokenizer functions */
t_token	*tokenize(const char *str, t_shell *shell, int *err);

/* Parser functions */
t_cmd	*parse_commands(t_token *tok, int *err);
t_cmd	*cmd_new(void);
int             is_redir(int type);
int             validate_syntax(t_token *tok);
void            add_redir(t_cmd *cmd, int type, const char *file);

/* Garbage collector */
void   *ft_malloc(size_t size);
void    ft_cleanup(t_malloc **m_list);

/* Environment functions */
t_env	*copy_env(char **envp);
char	*expand_var(const char *str, int *i, t_env *env, int last_status);
char    *expand_line(const char *line, t_shell *shell);

void	display_env(t_env *env);
/* Environment variable validation */
int		is_valid_var_name(const char *name);

/* Builtin commands */
int		builtin_echo(char **argv);
int		builtin_pwd(void);
int		builtin_cd(char **argv, t_env **env);
int		builtin_export(char **argv, t_env **env);
int		builtin_unset(char **argv, t_env **env);
int		builtin_env(t_env *env);
int		builtin_exit(char **argv, t_shell *shell);

/* Process and execution functions */
int	child_process(t_cmd *cmd, t_env *env, char **envp, int in_fd);
int	wait_for_children(int n, int *status);
int	exec_external(t_cmd *cmd, t_env *env, char **envp);
int	execute_builtin(t_cmd *cmd, t_env **env, t_shell *shell);
int	execute_single_cmd(t_cmd *cmd, t_env **env, char **envp, t_shell *shell);
int	execute_pipeline(t_cmd *cmds, t_env **env, char **envp, t_shell *shell);

/* Pipe functions */
int	open_pipe(int pipe_fd[2]);
void	close_pipe(int pipe_fd[2]);

/* Redirection functions */
int	redirect_input(const char *file);
int	redirect_output(const char *file, int append);
int	apply_all_redirs(t_cmd *cmd);

/* Signal handling */
void	setup_signals(void);
void	setup_heredoc_signals(void);

/* ************************************************************************** */
/*   GLOBALS                                                                 */
/* ************************************************************************** */
extern volatile sig_atomic_t	g_signal; /* For signal handling only */
/*heredoc handler*/
int handle_heredoc_execution(const char *delimiter, int quoted, t_shell *shell);

int		is_builtin(const char *cmd);

/* Tokenizer internals used across units */
int     tok_is_operator(char c);
int     tok_skip_spaces(const char *str, int i);
int     tok_get_token_type(const char *str, int i);
t_token *tok_token_new(char *value, int type, int quoted);
void    tok_token_add_back(t_token **lst, t_token *new);
char		*tok_expand_word(const char *str, int *i, t_tokctx *ctx);
int			spawn_child(int prev_fd, int *pipe_fd, t_cmd *cur, t_shell *shell);


#endif
