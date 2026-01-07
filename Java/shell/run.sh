#!/usr/bin/env bash

if [[ ! -f Shell.class || Shell.java -nt Shell.class ]]; then
    echo "Compiling..."
    javac Shell.java
fi

java Shell
