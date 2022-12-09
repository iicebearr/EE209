// operator Src Dest -> Dest = Dest {operator} Src
// result is usually stored in the second argument


/* Reading Assembly Language */
// %: register
// $: constant, immediate operand
// .: assembler directives
// #: comment  


/* Different Sizes in Assembly */
// Byte(b): 1 byte,  8 bit
// Word(w): 2 bytes, 16 bit
// Long(l): 4 bytes, 32 bit
//     (q): 8 bytes, 64 bit

// Intel is a little endian architecture


/* General purpose registers */
// used to store information for computation
// 64 bit: %rax
// 32 bit: %eax
// 16 bit: %ax
//  8 bit: %ah(high), %al(low)


/* special purpose registers */
// instruction pointer(%eip, %rip):
//      stores the location of the "next" instruction
// stack pointer register(%esp, %rsp):
//      address of the "top" of the stack
// base pointer register(%ebp, %rbp):
//      fixed reference point to access parameters,
//      "start" address of the "new" stack
// flag register(%eflags): 
//      stores the status of the operations


/* loading and storing data */
// variables are stored in memory
// BSS: global variables
// DATA: static local variables
// HEAP: dynamically allocated variables
// STACK: local variables, function variables

// loading: memory -> register
// storing: register -> memory

movl %eax, %ebx // general move instruction 
pushl %ebx // push %ebx into stack
popl  %ebx // pop stack into %ebx

// pushl %eax
subl $4, %esp
movl %ebx, (%esp)

// popl %eax
movl (%esp), %ebx
addl $4, %esp


/* addressing modes */
// 1. immediate addressing: data stored in the instruction itself, ex) $1
// 2. register addressing: data stored in a register, ex) %eax

// 3. direct addressing
movb 2000, %al // dereference memory address 2000 and store it in %al

// 4. indirect addressing
movl (%eax), %ebx // dereference memory address in %eax and store it in %ebx

// 5. base pointer addressing
movl 8(%eax), %ebx // dereference (8 + %eax) and store it in %ebx

// 6. indexed addressing
movl 8(%eax, %ebx, 4), %ecx // dereference (8 + %eax + %ebx * 4)

// format: "disp(base, index, scale)" -> disp + base + index * sclae
// base: register
// index: register 
// disp: None, 1, 2, 4, 8, assumed 0 when omitted (bytes), ex) (%eax, %ebx, 4)
// scale: 1, 2, 4, 8, assumed 1 when omitted (bytes), ex) (%eax, %ebx)


/* arithmetic instructions */
addl %eax, %ebx // %ebx = %ebx + %eax
subl %eax, %ebx // %ebx = %ebx - %eax

// signed
imull %ebx // %rax = %eax * %ebx
idivl %ebx // %rax = %eax / %ebx, %edx = %eax % %ebx

// unsigned
mull %ebx // %edx = %eax / %ebx
divl %ebx // %eax = %eax * %ebx

incl %eax // %eax = %eax + 1
decl %eax // %eax = %eax - 1

negl %eax // %eax = ~%eax + 1 (2's complement)

cmpl %eax, %ebx // %ebx - %eax


/* arithmetic instructions */
addl %eax, %ebx // %ebx = %ebx + %eax
subl %eax, %ebx // %ebx = %ebx - %eax

// signed
imull %ebx // %rax = %eax * %ebx
idivl %ebx // %rax = %eax / %ebx, %edx = %eax % %ebx

// unsigned
mull %ebx // %edx = %eax / %ebx
divl %ebx // %eax = %eax * %ebx

incl %eax // %eax = %eax + 1
decl %eax // %eax = %eax - 1

negl %eax // %eax = ~%eax + 1 (2's complement)

cmpl %eax, %ebx // %ebx - %eax


/* bitwise instructions */
andl %eax, %ebx // &
orl  %eax, %ebx // |
xorl %eax, %ebx // ^
notl %eax // ~

// logical shift
shll %eax // <<
shrl %eax // >>, w/o sign bit extension

// arithmetic shift
sall %eax // <<
sarl %eax // >>, w/ sign bit extension


/* condition codes */
// stores the status of the operation
// flags are not set by lea, inc, dec

// ZF: zero flag (result == 0)
// SF: sign flag (result < 0)
// CF: carry flag (unsigned overflow)
// OF: overflow flag (signed overflow)


/* jumps */
// go to a particular place in the code by changing the instruction pointer

// equality
je // ZF

// less, greater (signed)
jl  // SF ^ OF (negative xor overflow into positive)
jle // (SF ^ OF) | ZF
jg  // ~((SF ^ OF) | ZF)
jge // ~(SF ^ OF)
jo
jno

// below, above (unsigned)
jb  // CF
jbe // CF | ZF
ja  // ~(CF | ZF)
jae // ~CF
jc
jnc

// unconditional jump
jmp