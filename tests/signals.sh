#!/bin/sh
# Signal handling test
./minishell << EOF
sleep 5
# During sleep, send Ctrl-C and Ctrl-\ from another terminal
exit
EOF
