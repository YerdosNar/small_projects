#!/usr/bin/env bash

if [[ "$1" == "-o" || "$1" == "--only" ]]; then
    if [[ ! -f exec_timer || timer.c -nt exec_timer  ]]; then
        echo "COMPILING..."
        gcc -o exec_timer timer.c -Wall -Wextra -g
    else
        echo "SKIPPING COMPILATION"
    fi
elif [[ "$1" == "-c" || "$1" == "--clean" ]]; then
    rm exec_timer
elif [[ "$1" == "-h" || "$1" == "--help" ]]; then
    echo "./run.sh [options]"
    echo "Options: "
    echo "  -o, --only          Compile only, no execute"
    echo "  -c, --clean         Clean up"
    echo "  -h, --help          Print this message"
else
    if [[ ! -f exec_timer || timer.c -nt exec_timer  ]]; then
        echo "COMPILING..."
        gcc -o exec_timer timer.c -Wall -Wextra -g
    else
        echo "SKIPPING COMPILATION"
    fi
    ./exec_timer "$@"
fi
