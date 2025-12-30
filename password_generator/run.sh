#!/usr/bin/env bash

if [ "$1" == "asm" ];then
    nasm generator.asm -f elf64 -o generator.o
    gcc -o generator generator.o -no-pie
    ./generator
    rm generator generator.o
else
    gcc -o generator generator.c -Wall -Wextra
    ./generator "$@"
    rm generator
fi

