#!/usr/bin/env bash

gcc -o gen generate.c -Wall -Wextra

./gen $1 $2 $3 $4 $5
