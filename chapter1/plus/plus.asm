; nasm -f elf64 plus.asm
; gcc -o plus plus.o 

section .data
section .bss
section .text

global main

plus:
	push rbp	; Remember base pointer
	mov rbp, rsp	; rbp holds stack pointer
	mov eax, esi	; To add eax, edx, move values from main func.
	mov edx, edi
	add eax, edx
	pop rbp
	ret		; Return eax which holds the sum.

main:
	push rbp
	mov rbp, rsp
	mov esi, 0x4
	mov edi, 0x3
	call plus
	pop rbp
	ret
