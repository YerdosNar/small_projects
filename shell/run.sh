#!/usr/bin/env bash

gcc -o shell shell.c -Wall -Wextra
./shell "$@"

rm shell
