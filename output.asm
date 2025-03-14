default rel

section .text
	global main

main:
	;prepare stack
	push rbp
	mov rbp, rsp
	;declaring a
	sub rsp, 8
	;assigning a
	mov r10, 749
	mov [rbp-8], r10
	;declaring b
	sub rsp, 8
	;assigning b
	mov r10, 568
	mov [rbp-16], r10
	;assigning a
	mov rax, [rbp-8]
	mov rcx, [rbp-16]
	;bitwise xor (^)
	xor rax, rcx
	mov [rbp-8], rax
	;assigning b
	mov rax, [rbp-8]
	mov rcx, [rbp-16]
	;bitwise xor (^)
	xor rax, rcx
	mov [rbp-16], rax
	;assigning a
	mov rax, [rbp-8]
	mov rcx, [rbp-16]
	;bitwise xor (^)
	xor rax, rcx
	mov [rbp-8], rax
	;get return value
	mov rax, [rbp-8]
	;restore stack
	mov rsp, rbp
	pop rbp
	ret
