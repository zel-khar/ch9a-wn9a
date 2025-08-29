#!/bin/sh
# Error and edge case test
./minishell << EOF

|
>
<
ls /no/such/dir
cat < /no/perm/file
exit
EOF
