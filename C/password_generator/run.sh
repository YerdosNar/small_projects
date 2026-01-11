#!/usr/bin/env bash

if [ "$1" == "asm" ];then
    shift 1
    nasm generator.asm -f elf64 -o generator.o
    gcc -o generator generator.o -no-pie
    echo "$@"
    ./generator "$@"

    rm generator generator.o
else
    gcc -o generator generator.c -Wall -Wextra
    ./generator "$@"
    rm generator
fi

