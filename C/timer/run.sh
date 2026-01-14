#!/usr/bin/env bash

if [[ ! -f exec_timer || timer.c -nt exec_timer  ]]; then
    gcc -o exec_timer timer.c -Wall -Wextra -g
fi

./exec_timer "$@"

rm exec_timer
