# --------------------------------------------------------------------
# dc.s
#
# Desk Calculator (dc) (x86-64)
#
# Student ID: 20210013
# --------------------------------------------------------------------

    .equ   BUFFERSIZE, 32
    .equ   OPSIZE, 16
    .equ   HALFOPSIZE, 8
    .equ   BYTE, 1
    .equ   INTMIN, -2147483648
    .equ   EOF, -1

# --------------------------------------------------------------------

.section ".rodata"

scanfFormat:
     .asciz "%s"

printfFormat:
     .asciz "%d\n"

stackemptyString:
     .asciz "dc: stack empty\n"

overflowString:
     .asciz "dc: overflow happens\n"

zeroDivString:
     .asciz "dc: divide by zero\n"

zeroRemString:
     .asciz "dc: remainder by zero\n"

# --------------------------------------------------------------------

    .section ".data"

# --------------------------------------------------------------------

    .section ".bss"

# --------------------------------------------------------------------
    .section ".text"

    # -------------------------------------------------------------
    # int powerfunc(int base, int exponent)
    # Runs the power function.  Returns result.
    # -------------------------------------------------------------

    .globl	powerfunc
    .type   powerfunc, @function

    # base is stored in %rdi
    # exponent is stored in %rsi

powerfunc:
    movq    $1, %rax # base case when exponent is zero
    jmp     checkExp
loopPow:
    imull   %edi
    jo      overflow
    decq    %rsi
checkExp:
    cmpq    $0, %rsi
    je      endloopPow
    jmp     loopPow
endloopPow:
    ret


# -------------------------------------------------------------
# int main(void)
# Runs desk calculator program.  Returns 0.
# -------------------------------------------------------------

    .text
    .globl  main
    .type   main, @function

main:
    # save value of %rbp and move %rbp
    pushq   %rbp
    movq    %rsp, %rbp

    # allocate memory for buffer
    subq    $BUFFERSIZE, %rsp


.input:
    # scan user input into buffer
    leaq    scanfFormat, %rdi
    leaq    -BUFFERSIZE(%rbp), %rsi
    movb    $0, %al
    call    scanf


    # check if user input == EOF
    cmpq    $EOF, %rax
    je	    .quit


    # check if first character of user input is digit 
    movb    -BUFFERSIZE(%rbp), %bl
    movzbq   %bl, %rdi
    call isdigit
    cmpq $0, %rax
    jne pushInput


    # check if first character of user input is '_'
    movb    -BUFFERSIZE(%rbp), %bl
    cmpb    $'_', %bl
    jne endif_

    leaq    -BUFFERSIZE(%rbp), %r12
    movb    BYTE(%r12), %bl # skip '_'
    movzbq   %bl, %rdi
    call isdigit # check if digit after '_'
    cmpq $0, %rax
    je .input # not digit after '_'

    jmp      pushNegInput
endif_:


    # check if user input == 'q'
    movb    -BUFFERSIZE(%rbp), %bl
    cmpb    $'q', %bl
    je     .quit


    # check if user input == 'p'
    movb    -BUFFERSIZE(%rbp), %bl
    cmpb    $'p', %bl
    jne     endifp

    leaq    -BUFFERSIZE(%rbp), %rbx
    cmpq    %rbx, %rsp # check if stack is empty
    je      stackempty
    
    leaq    printfFormat, %rdi
    movq    (%rsp), %rsi
    movb    $0, %al
    call    printf
    jmp     .input
endifp:


    # check if user input == 'f'
    movb    -BUFFERSIZE(%rbp), %bl
    cmpb    $'f', %bl
    jne     endiff

    movq    $0, %r12 # initialize index
    jmp     checkEnd # check if end of stack

loopPrint:
    leaq    printfFormat, %rdi
    movq    (%rsp,%r12,HALFOPSIZE), %rsi
    movb    $0, %al
    call    printf
    addq    $2, %r12 # increment index

checkEnd:
    leaq    -BUFFERSIZE(%rbp), %r13
    leaq    (%rsp,%r12,HALFOPSIZE), %r14
    cmpq    %r13, %r14
    jl      loopPrint

    jmp     .input
endiff:


    # check if user input == 'c'
    movb    -BUFFERSIZE(%rbp), %bl
    cmpb    $'c', %bl
    jne     endifc
    leaq    -BUFFERSIZE(%rbp), %rsp # clear stack
    jmp     .input
endifc:


    # check if user input == '+'
    movb    -BUFFERSIZE(%rbp), %bl
    cmpb    $'+', %bl
    jne     endifadd

    leaq    -BUFFERSIZE(%rbp), %rbx
    leaq    -OPSIZE(%rbx), %r12 
    cmpq    %r12, %rsp # check if stack has at least 2 operands
    jae     stackempty

    movq    (%rsp), %r14 # pop second operand
    addq    $OPSIZE, %rsp
    movq    (%rsp), %r13 # pop first operand
    addq    $OPSIZE, %rsp

    addl    %r14d, %r13d # add two operands
    jo      overflow
    subq    $OPSIZE, %rsp # push result into stack
    movq    %r13, (%rsp)
    jmp     .input
endifadd:


    # check if user input == '-'
    movb    -BUFFERSIZE(%rbp), %bl
    cmpb    $'-', %bl
    jne     endifsub

    leaq    -BUFFERSIZE(%rbp), %rbx
    leaq    -OPSIZE(%rbx), %r12 
    cmpq    %r12, %rsp # check if stack has at least 2 operands
    jae     stackempty

    movq    (%rsp), %r14 # pop second operand
    addq    $OPSIZE, %rsp
    movq    (%rsp), %r13 # pop first operand
    addq    $OPSIZE, %rsp

    subl    %r14d, %r13d # subtact two operands
    jo      overflow
    subq    $OPSIZE, %rsp # push result into stack
    movq    %r13, (%rsp)
    jmp     .input
endifsub:


    # check if user input == '*'
    movb    -BUFFERSIZE(%rbp), %bl
    cmpb    $'*', %bl
    jne     endifmul

    leaq    -BUFFERSIZE(%rbp), %rbx
    leaq    -OPSIZE(%rbx), %r12 
    cmpq    %r12, %rsp # check if stack has at least 2 operands
    jae     stackempty

    movq    (%rsp), %r14 # pop second operand
    addq    $OPSIZE, %rsp
    movq    (%rsp), %rax # pop first operand
    addq    $OPSIZE, %rsp

    imull   %r14d # multiply two operands
    jo      overflow
    subq    $OPSIZE, %rsp # push result into stack
    movq    %rax, (%rsp)
    jmp     .input
endifmul:


    # check if user input == '/'
    movb    -BUFFERSIZE(%rbp), %bl
    cmpb    $'/', %bl
    jne     endifdiv

    leaq    -BUFFERSIZE(%rbp), %rbx
    leaq    -OPSIZE(%rbx), %r12 
    cmpq    %r12, %rsp # check if stack has at least 2 operands
    jae     stackempty

    movq    (%rsp), %r14 # pop second operand
    addq    $OPSIZE, %rsp

    cmpl    $0, %r14d
    je      zeroDiv # check if second operand is zero

    movq    (%rsp), %rax # pop first operand
    addq    $OPSIZE, %rsp

    cmpl    $-1, %r14d # check for exception case
    jne     notDivException
    cmpl    $INTMIN, %eax
    je      overflow 

notDivException:
    movq    $0, %rdx
    cdq
    idivl   %r14d # divide two operands
    subq    $OPSIZE, %rsp # push result into stack
    movq    %rax, (%rsp)
    jmp     .input
endifdiv:


    # check if user input == '%'
    movb    -BUFFERSIZE(%rbp), %bl
    cmpb    $'%', %bl
    jne     endifrem

    leaq    -BUFFERSIZE(%rbp), %rbx
    leaq    -OPSIZE(%rbx), %r12 
    cmpq    %r12, %rsp # check if stack has at least 2 operands
    jae     stackempty

    movq    (%rsp), %r14 # pop second operand
    addq    $OPSIZE, %rsp

    cmpl    $0, %r14d
    je      zeroRem # check if second operand is zero

    movq    (%rsp), %rax # pop first operand
    addq    $OPSIZE, %rsp

    cmpl    $-1, %r14d # check for exception case
    jne     notRemException
    cmpl    $INTMIN, %eax
    jne     notRemException 

    subq    $OPSIZE, %rsp # push 0 into stack
    movq    $0, (%rsp)
    jmp     .input

notRemException:
    movq    $0, %rdx
    cdq
    idivl   %r14d # divide two operands
    subq    $OPSIZE, %rsp # push result into stack
    movq    %rdx, (%rsp)
    jmp     .input
endifrem:


    # check if user input == '^'
    movb    -BUFFERSIZE(%rbp), %bl
    cmpb    $'^', %bl
    jne     endifpow

    leaq    -BUFFERSIZE(%rbp), %rbx
    leaq    -OPSIZE(%rbx), %r12 
    cmpq    %r12, %rsp # check if stack has at least 2 operands
    jae     stackempty

    movq    (%rsp), %rsi # pop second operand
    addq    $OPSIZE, %rsp
    movq    (%rsp), %rdi # pop first operand
    addq    $OPSIZE, %rsp

    call    powerfunc

    subq    $OPSIZE, %rsp # push result into stack
    movq    %rax, (%rsp)
    jmp     .input
endifpow:


    # user input is meaningless
    jmp     .input



pushInput:
    # push number into stack
    leaq    -BUFFERSIZE(%rbp), %rdi
    call    atoi # change str to int
    subq    $OPSIZE, %rsp # push operand into stack
    movq    %rax, (%rsp)
    jmp     .input


pushNegInput:
    # push negative number into stack
    leaq    -BUFFERSIZE(%rbp), %rbx
    leaq    BYTE(%rbx), %rdi # skip '_'
    call    atoi # change str to int
    negq    %rax # change sign to negative
    subq    $OPSIZE, %rsp # push operand into stack
    movq    %rax, (%rsp)
    jmp     .input


stackempty:
    movq    stderr, %rdi   
    leaq    stackemptyString, %rsi
    movb    $0, %al
    call    fprintf
    jmp     .input


overflow:
    movq    stderr, %rdi   
    leaq    overflowString, %rsi
    movb    $0, %al
    call    fprintf
    jmp     .quit


zeroDiv:
    movq    stderr, %rdi   
    leaq    zeroDivString, %rsi
    movb    $0, %al
    call    fprintf
    jmp     .quit


zeroRem:
    movq    stderr, %rdi   
    leaq    zeroRemString, %rsi
    movb    $0, %al
    call    fprintf
    jmp     .quit


    .quit:
    movq    $0, %rax
    addq    $BUFFERSIZE, %rsp
    movq    %rbp, %rsp
    popq    %rbp
    ret
