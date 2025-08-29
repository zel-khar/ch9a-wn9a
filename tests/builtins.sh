#!/bin/sh
# Built-in commands test
./minishell << EOF
echo Hello World
echo -n NoNewline
echo "Quoted $USER"
pwd
cd /
pwd
cd /doesnotexist
export FOO=bar
echo $FOO
unset FOO
echo $FOO
env
exit
EOF
