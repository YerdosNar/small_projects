#!/usr/bin/env bash

if [[ $# -eq 0 || "$1" == "--help" || "$1" == "-h" ]]; then
    echo "+-----------------------------------------+"
    echo "| Usage: ./run.sh [opts] <filename>       |"
    echo "+-----------------------------------------+"
    echo "| Options:                                |"
    echo "|     -h, --help      Help message        |"
    echo "|     -c, --clean     Clean up            |"
    echo "|     -o, --only <f>  Only compile        |"
    echo "|     [filename]      Compile and execute |"
    echo "+-----------------------------------------+"
    echo "| Example:                                |"
    echo "|          ./run.sh main.c                |"
    echo "+-----------------------------------------+"
    exit
fi

compile() {
    filename="$1"
    exe_name="exe_${filename%.*}"
    if [[ -f "$exe_name" && "$exe_name" -nt "$filename" ]];then
        echo "Skipping compilation, file already compiled"
    else
        echo "Compiling $filename..."
        if ! gcc -o $exe_name $filename -Wall -Wextra -lm; then
            exit 1
        fi
    fi
}

if [[ "$1" == "-c" || "$1" == "--clean" ]]; then
    echo "Cleaning..."
    rm exe_*
elif [[ "$1" == "-o" || "$1" == "--only" ]]; then
    compile "$2"
else
    compile "$1"

    ./$exe_name
    rm $exe_name
fi

echo ""
echo "=========================="
echo "       +-------+"
echo "       | Done! |"
echo "       +-------+"
