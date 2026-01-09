# Simple password generator

- ## Flags:
    - #### -l, --length
        to set the length of the password
    - #### -w, --write
        to save password in a file

- ## Compile:
    ```bash
    gcc -o generator generator.c
    ```

- ## Assembly CISC x86_64:
    ```bash
    nasm -f elf64 generator.asm -o generator.o
    gcc -o generator generator.o -no-pie
    ```

- ## Or you can run:
    ```bash
    ./run.sh     -- to run C version
    ./run.sh asm -- to run assembly version
    ```
