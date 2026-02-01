#!/usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Enter the filename to compile"
    exit
fi

filename=$1
exe_name=${filename%.*}
gcc -o $exe_name $filename -Wall -Wextra -lm

./$exe_name

rm $exe_name
