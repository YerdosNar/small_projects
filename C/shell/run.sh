#!/usr/bin/env bash

if [[ ! -f exec_shell || shell.c -nt exec_shell ]]; then
    gcc -o exec_shell shell.c -Wall -Wextra
fi

./exec_shell "$@"
