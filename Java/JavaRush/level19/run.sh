#!/usr/bin/env bash

compile() {
    local filename="$1"
    echo "Compiling: $filename"
    javac $filename
}

if [[ $# == 0 || "$1" == "-h" || "$1" == "--help" ]]; then
    echo "Usage: $0 <filename>"
    exit 0
elif [[ "$1" == "-l" || "$1" == "--list" ]]; then
    echo "Java Files: "
    for file in ls *.java; do
        echo "$file"
    done
elif [[ "$1" == "-c" || "$1" == "--clean" ]]; then
    for file in *.class; do
        if [ -f "$file" ]; then
            echo "Cleaning..."
            rm -f *.class *.bin *.txt
            exit 0
        else
            echo "All clean!"
        fi
    done
elif [[ "$1" == "-a" || "$1" == "--all" ]]; then
    for file in *.java; do
        compile "$file"
    done
else
    filename="$1"
    compile "$filename"
    java "${filename%.*}"
fi
