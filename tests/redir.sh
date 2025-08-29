#!/bin/sh
# Redirection scenarios test
./minishell << EOF
echo hi > out.txt
cat < out.txt
echo append >> out.txt
cat < out.txt
cat < missing.txt
exit
EOF
