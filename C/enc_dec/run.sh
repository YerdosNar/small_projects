#!/usr/bin/env bash

if [[ ! -f exec_enigma && enigma.c -nt exec_enigma ]]; then
    gcc -o exec_enigma enigma.c
fi

./exec_enigma "$@"

rm exec_enigma
