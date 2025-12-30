section .data
    fmt_char db "%c", 0
    newline  db 10, 0

section .text
    global main
    extern printf
    extern rand
    extern srand
    extern time

main:
    push    rbp     ; save base pointer
    mov     rbp, rsp; setup stack frame

    ; srand(time(NULL)
    mov     rdi, 0  ; arg for time(): NULL
    call time       ; return `time` in RAX
    mov rdi, rax    ; move time result to first arg RDI
    call srand      ; Call srand

    ; loop counter init (i = 0)
    ; using RBX, `callee-saved`
    ; funcs like `printf`, `rand` cannot chage RBX
    xor rbx, rbx    ; this is how we get zero

loop_start:
    cmp rbx,10      ; comapring i to 10
    jge loop_end    ; jge(jump grater or equal) i>=10

    ; generate either uppercase or lowercase
    ; if RAX's last bit 1, then uppercase
    ; else lowercase
    call rand       ; return random int in RAX
    test rax, 1
    jz generate_lowercase

generate_uppercase:
    ; char c = rand() % 26 + 65
    call rand       ; RAX random number
    xor rdx, rdx    ; RDX set to zero
    mov rcx, 26     ; divisor
    div rcx         ; divide RAX by 26, remainder in RDX
    add rdx, 65     ; add 65 to go to uppercase letters
    jmp print_char

generate_lowercase:
    ; char c = rand() % 26 + 97
    call rand       ; RAX random number
    xor rdx, rdx    ; RDX set to zero
    mov rcx, 26     ; divisor
    div rcx         ; divide RAX by 26, remainder in RDX
    add rdx, 97     ; add 97 to go to uppercase letters
    jmp print_char

; generate_number:
;     ; char c = rand() % 10 + 47
;     call rand       ; RAX rand number
;     xor rdx, rdx
;     mov rcx, 10
;     div rcx
;     add rdx, 47
;     jmp print_char

print_char:
    ; printf("%c", c)
    mov rdi, fmt_char ; first arg: "%c"
    mov rsi, rdx      ; second argc: the char `c`
    xor rax, rax    ; RAX = 0
    call printf

    inc rbx         ; i++
    jmp loop_start  ; repeat loop

loop_end:
    ; printf("\n")
    mov rdi, fmt_char
    mov rsi, 10
    xor rax, rax
    call printf

    ; return 0
    xor rax, rax
    pop rbp
    ret
