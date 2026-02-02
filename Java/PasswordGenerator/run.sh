#!/usr/bin/env bash

javac Generate.java
java Generate "$@"

rm -f Generate.class
