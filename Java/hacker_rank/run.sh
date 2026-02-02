#!/usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "+--------------------------------------+"
    echo "| Usage: ./run.sh <filename>           |"
    echo "+--------------------------------------+"
    echo "| Options:                             |"
    echo "|     -c/--clean Clean all .class files|"
    echo "+--------------------------------------+"
    echo "| Example:                             |"
    echo "|          ./run.sh main.c             |"
    echo "+--------------------------------------+"
    exit
elif [[ "$1" == "-c" || "$1" == "--clean" ]]; then
    for file in *.class; do
        if [ -f $file ]; then
            echo "Found: $file"
        else
            echo "Nothing found..."
            exit
        fi
    done
    echo -e "\033[1;31mCLEANING...\033[0m"
    rm *.class
    exit
fi

filename=$1
exe_name=${filename%.*}
if [[ -f "$exe_name" && "$exe_name" -nt "$filename" ]];then
    echo "Skipping compilation, file already compiled"
else
    javac $filename
    java $exe_name

    rm ${exe_name}.class
fi
echo ""
echo "=========================="
echo "       +-------+"
echo "       | Done! |"
echo "       +-------+"
