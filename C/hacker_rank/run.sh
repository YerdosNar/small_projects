#!/usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "+----------------------------+"
    echo "| Usage: ./run.sh <filename> |"
    echo "+----------------------------+"
    echo "| Example:                   |"
    echo "|          ./run.sh main.c   |"
    echo "+----------------------------+"
    exit
fi

filename=$1
exe_name=${filename%.*}
if [[ -f "$exe_name" && "$exe_name" -nt "$filename" ]];then
    echo "Skipping compilation, file already compiled"
else
    gcc -o $exe_name $filename -Wall -Wextra -lm
    ./$exe_name

    rm $exe_name
fi
echo ""
echo "=========================="
echo "       +-------+"
echo "       | Done! |"
echo "       +-------+"
