/* special purpose registers */
// stack pointer register(%esp, %rsp):
//      address of the "top" of the stack
// base pointer register(%ebp, %rbp):
//      fixed reference point to access parameters,
//      "start" address of the "new" stack


/* implementation of "push"/"pop" */
// stack grows from high memory address to low memory address
pushl %ebx // push EBX into stack
popl  %ebx // pop stack into EBX

// pushl %eax
subl $4, %esp
movl %ebx, (%esp)

// popl %eax
movl (%esp), %ebx
addl $4, %esp


/* stack frame */
// stack has one stack frame per active function invocation
// ESP points to the top (low memory) of the current stack frame
// EBP points to the bottom (high memory) of current stack frame

// stack frame contains:
//  (low memory)
//      local variables
//      callee saved register values
//      old EBP
//      old EIP (return address)
//      parameters to be passed to the callee function
//      caller saved register values
//  (high memory)


/* caller-save/callee-save registers*/
// caller-save registers: saved in stack and restored by caller
// callee-save registers: saved in stack and restored by callee 

// caller can assume that callee-save registers will not be changed by the callee


/* return values */
// Integral type or pointer: Store return value in EAX
// Floating-point type: Store return value in floating-point register
// Structure: Store return value on stack


/* implementation of functions in assembly */
//---------------------------------------------------------------------------
/* caller */
//---------------------------------------------------------------------------
/* 1. save caller-save registers */

/* 2. push function parameters */
pushl parameters

/* 3. implementation of "call"*/
// push EIP into stack and jump
// call address
pushl %eip // store old EIP
jmp address

//---------------------------------------------------------------------------
/* callee */
//---------------------------------------------------------------------------
/* 1. prolog */
// prolog
pushl %ebp // store old EBP
movl  %esp, %ebp // set new EBP to ESP ("start" address of the "new" stack)

/* 2. save callee-save registers */

/* 3. perform task */
// this may change ESP as local variables are stored in the stack

/* 4. copy return value to EAX */

/* 5. restore callee-save registers */

/* 6. epilog */
movl %ebp, %esp // restore ESP to its value before performing task
popl %ebp // restore old EBP

/* 7. implementation of "ret" */
// pop stack to EIP
// ret
popl %eip

//---------------------------------------------------------------------------
/* caller */
//---------------------------------------------------------------------------
/* 1. pop function parameters */
pushl parameters

/* 2. restore caller-save registers */