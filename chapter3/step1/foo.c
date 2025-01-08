/* Global variable is same to function in Assembly level.
 * In default, global variable will be assigned to memory which
 * prevents execution.
 * -- Related words
 *  -> DEP(Data Execution Prevention)
 *  -> ASLR(Address Space layout Randomization)
 * Substantially, function and global variable are just data on memory.
 */


/* `main` indentifier is defined as global variable */
/* "\x48\xc7\xc0\x2a\x00\x00\x00\xc3" is a machine code in x86-64 */
char main[] = "\x48\xc7\xc0\x2a\x00\x00\x00\xc3";

/*
 * > cc -c foo.c
 * ❯ objdump -D -M intel foo.o
 * foo.o:     file format elf64-x86-64
 *
 *
 * Disassembly of section .data:
 *
 * 0000000000000000 <main>:
 * 0:   48 c7 c0 2a 00 00 00    mov    rax,0x2a
 * 7:   c3                      ret
 * ...
 *
 */
