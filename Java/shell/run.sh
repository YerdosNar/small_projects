#!/usr/bin/env bash

if [[ ! -f Shell.class || Shell.java -nt Shell.class ]]; then
    javac Shell.java
fi

java Shell
