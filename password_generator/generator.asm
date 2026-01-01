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

; To prevent
; First char != number
first_iteration:
    call rand
    test rax, 1
    jz generate_uppercase
    jmp generate_lowercase

    ; loop counter init (i = 0)
    ; using RBX, `callee-saved`
    ; funcs like `printf`, `rand` cannot chage RBX
    xor rbx, rbx    ; this is how we get zero


loop_start:
    cmp rbx,20      ; comapring i to 10
    jge loop_end    ; jge(jump grater or equal) i>=10

    ; Generate type: 0=lower, 1=upper, 2=generate_number
    call rand       ; return random int in RAX
    xor rdx, rdx    ; RDX = 0
    mov rcx, 3      ; divide to 3
    div rcx         ; remainder in RDX

    ; if 0 -> generate_lowercase
    cmp rdx, 0      ; compare with 0
    je generate_lowercase ;

    ; if 1 -> generate_uppercase
    cmp rdx, 1
    je generate_uppercase ;

    ; else -> generate_number
    jmp generate_number


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

generate_number:
    ; char c = rand() % 10 + 48
    call rand       ; RAX rand number
    xor rdx, rdx
    mov rcx, 10
    div rcx
    add rdx, 48
    jmp print_char

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
