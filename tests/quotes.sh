#!/bin/sh
# Quote handling test
./minishell << EOF
echo "double $USER"
echo 'single $USER'
echo "mix 'quotes' $USER"
echo 'mix "quotes" $USER'
exit
EOF
