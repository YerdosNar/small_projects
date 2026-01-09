#!/usr/bin/env bash

if [ "$1" == "asm" ];then
    nasm generator.asm -f elf64 -o generator.o
    gcc -o generator generator.o -no-pie
    read -p "Would you to give flags: " flag
    if [ -z $flag ]; then
        echo "Nothing is received: Running noflags"
        ./generator
    else
        read -p "Flag '$flag' received: give following: " length
        ./generator "$flag" "$length"
    fi
    rm generator generator.o
else
    gcc -o generator generator.c -Wall -Wextra
    ./generator "$@"
    rm generator
fi

