/* Differences between X86-64 and IA-32 */
// 1. more registers

// 2. first 6 arguments are stored in registers
//    RDI, RSI, RDX, RCX, R8, R9

// 3. 16 byte alignment before calling functions

// 4. When calling functions with a variable number of arguments
//    %al must set to the number of vector registers used