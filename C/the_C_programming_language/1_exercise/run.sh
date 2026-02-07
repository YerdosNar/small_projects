#!/usr/bin/env bash

filename="$1"
exec_file="${filename%.*}"
gcc $filename -o $exec_file
./$exec_file

rm -f $exec_file
