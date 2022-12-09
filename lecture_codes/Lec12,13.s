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

// Intel is a little endianarchitecture


/* General purpose registers */
// used to store information for computation
// 64 bit: %rax
// 32 bit: %eax
// 16 bit: %ax
//  8 bit: %ah(high), %al(low)


/* special purpose registers */
// instruction pointer(%eip, %rip): stores the location of the "next" instruction


/* loading and storing data */
// variables are stored in memory
// BSS: global variables
// DATA: static local variables
// HEAP: dynamically allocated variables
// STACK: local variables, function variables

// loading: memory -> register
// storing: register -> memory


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
addl %eax, %ebx
subl %eax, %ebx

imull %ebx
mull  %ebx
idivl %ebx
divl  %ebx

incl %eax
decl %eax

negl %eax

cmpl %eax, %ebx
