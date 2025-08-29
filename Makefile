NAME		= minishell
CC			= cc
CFLAGS		= -Wall -Wextra -Werror
RM			= rm -f

SRCS_DIR	= src/
INCS_DIR	= inc/
OBJS_DIR	= obj/

SRCS		= \
				$(SRCS_DIR)main.c \
				$(SRCS_DIR)core/signals.c \
				$(SRCS_DIR)core/gc.c \
				$(SRCS_DIR)env/env.c \
				$(SRCS_DIR)tokenizer/tokenizer_core.c \
				$(SRCS_DIR)tokenizer/tokenizer_utils.c \
				$(SRCS_DIR)tokenizer/tokenizer_expand.c \
				$(SRCS_DIR)tokenizer/tokenizer_expand_utils.c \
				$(SRCS_DIR)tokenizer/expansion.c \
				$(SRCS_DIR)parser/parser.c \
				$(SRCS_DIR)parser/parser_utils.c \
				$(SRCS_DIR)parser/parser_validation.c \
				$(SRCS_DIR)exec/execution.c \
				$(SRCS_DIR)exec/execute_pipeline.c \
				$(SRCS_DIR)exec/pipeline_utils.c \
				$(SRCS_DIR)exec/process.c \
				$(SRCS_DIR)exec/pipes.c \
				$(SRCS_DIR)exec/redirection.c \
				$(SRCS_DIR)builtins/builtins.c \
				$(SRCS_DIR)builtins/export.c \
				$(SRCS_DIR)builtins/unset.c \
				$(SRCS_DIR)builtins/cd.c \
				$(SRCS_DIR)builtins/exit.c

OBJS		= $(SRCS:$(SRCS_DIR)%.c=$(OBJS_DIR)%.o)

LIBFT_DIR	= libft/
LIBFT		= $(LIBFT_DIR)libft.a

INCLUDES	= -I$(INCS_DIR) -I$(LIBFT_DIR)

all:		$(LIBFT) $(NAME)

$(LIBFT):
			@$(MAKE) -C $(LIBFT_DIR)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -L$(LIBFT_DIR) -lft -lreadline


$(OBJS_DIR)%.o: $(SRCS_DIR)%.c
				@mkdir -p $(dir $@)
			$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
			$(RM) -r $(OBJS_DIR)
			@$(MAKE) -C $(LIBFT_DIR) clean

fclean:		clean
			$(RM) $(NAME)
			@$(MAKE) -C $(LIBFT_DIR) fclean

re:			fclean all

.PHONY:		all clean fclean re
