default rel

section .text
	global main
	extern printf

main:
	;prepare stack
	push rbp
	mov rbp, rsp
	;declaring str1
	sub rsp, 8
	;declaring str2
	sub rsp, 8
	;declaring result
	sub rsp, 8
	;assigning str1
	mov rax, 24523985906328904
	mov [rbp-8], rax
	;assigning str2
	mov rax, 11138535027311
	mov [rbp-16], rax
	;assigning result
	;preparing to call printf
	;preparing arguments
	;reference operator (&)
	lea rcx, [rbp-8]
	mov rdx, 0
	;preparing stack
	push rbx
	mov rbx, rsp
	and rsp, -16
	sub rsp, 32
	call printf
	mov rsp, rbx
	pop rbx
	mov [rbp-24], rax
	;assigning result
	;preparing to call printf
	;preparing arguments
	;reference operator (&)
	lea rcx, [rbp-16]
	mov rdx, 0
	;preparing stack
	push rbx
	mov rbx, rsp
	and rsp, -16
	sub rsp, 32
	call printf
	mov rsp, rbx
	pop rbx
	mov [rbp-24], rax
	;get return value
	mov rax, 0
	;restore stack
	mov rsp, rbp
	pop rbp
	ret
