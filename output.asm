default rel

section .text
	global main

fib:
	;prepare stack
	push rbp
	mov rbp, rsp
	;if statement
	;evaluate condition
	mov rax, [rbp+16]
	mov rcx, 1
	;less than or equal (<=)
	cmp rax, rcx
	setle cl
	movzx rax, cl
	test rax, rax
	jz .IF_END0000016e573c9660
	;get return value
	mov rax, [rbp+16]
	;restore stack
	mov rsp, rbp
	pop rbp
	ret
.IF_END0000016e573c9660:
	;get return value
	;preparing to call fib
	push rcx
	mov rax, [rbp+16]
	mov rcx, 1
	;subtraction (-)
	sub rax, rcx
	push rax
	call fib
	;reclaim stack space
	add rsp, 8
	pop rcx
	;preparing to call fib
	push rax
	mov rax, [rbp+16]
	mov rcx, 2
	;subtraction (-)
	sub rax, rcx
	push rax
	call fib
	;reclaim stack space
	add rsp, 8
	mov rcx, rax
	pop rax
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
	;get return value
	;preparing to call fib
	mov r10, 10
	push r10
	call fib
	;reclaim stack space
	add rsp, 8
	;restore stack
	mov rsp, rbp
	pop rbp
	ret
