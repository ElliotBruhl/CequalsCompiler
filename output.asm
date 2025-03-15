default rel

section .text
	global main

main:
	;prepare stack
	push rbp
	mov rbp, rsp
	;get return value
	mov rax, 0
	;restore stack
	mov rsp, rbp
	pop rbp
	ret
