NAME	= libft.a
CC		= cc
CFLAGS	= -Wall -Wextra -Werror
AR		= ar rcs
RM		= rm -f

SRCS	= ft_split.c ft_strjoin.c ft_itoa.c ft_putendl_fd.c ft_strndup.c ft_atoi.c ft_isalnum.c ft_strcmp.c ft_putstr_fd.c ft_putchar_fd.c ft_isalpha.c ft_strlen.c ft_strdup.c

OBJS	= $(SRCS:.c=.o)

all:	$(NAME)

$(NAME):	$(OBJS)
			$(AR) $(NAME) $(OBJS)

%.o:		%.c
			$(CC) $(CFLAGS) -c $< -o $@

clean:
			$(RM) $(OBJS)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
