#!/bin/sh
# Pipe combinations test
./minishell << EOF
echo foo | cat -e
echo foo | grep f | wc -l
ls | grep minishell
exit
EOF
