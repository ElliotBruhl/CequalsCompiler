default rel

section .text
	global main

add:
	;prepare stack
	push rbp
	mov rbp, rsp
	;get return value
	mov rax, [rbp-8] 		;gotta fix this --- parameters are passed above the stack frame
	mov rcx, [rbp-16]
	;addition (+)
	add rax, rcx
	;restore stack
	mov rsp, rbp
	pop rbp
	ret

main:
	;prepare stack
	push rbp
	mov rbp, rsp
	;declaring a
	sub rsp, 8
	;declaring b
	sub rsp, 8
	;declaring c
	sub rsp, 8
	;assigning a
	mov r10, 1
	mov [rbp-8], r10
	;assigning b
	mov r10, 2
	mov [rbp-16], r10
	;assigning c
	;preparing to call add
	push qword [rbp-8]
	push qword [rbp-16]
	call add
	;reclaim stack space
	add rsp, 16
	mov [rbp-24], rax
	;get return value
	mov rax, [rbp-24]
	;restore stack
	mov rsp, rbp
	pop rbp
	ret

