#!/usr/bin/env bash

gcc -o gen generate.c -Wall -Wextra

./gen "$@"
