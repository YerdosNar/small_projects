#!/usr/bin/env bash

filename=$1
exe_name=${filename%.*}
gcc -o $exe_name $filename -Wall -Wextra

./$exe_name

rm $exe_name
