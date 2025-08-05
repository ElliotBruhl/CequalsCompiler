default rel

section .text
	global main
	extern printf
	extern scanf

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
	jz .IF_END000002307d6aa0e0
	;get return value
	mov rax, [rbp+16]
	;restore stack
	mov rsp, rbp
	pop rbp
	ret
.IF_END000002307d6aa0e0:
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
	;declaring n
	sub rsp, 8
	;declaring result
	sub rsp, 8
	;declaring formatStr1
	sub rsp, 8
	;declaring formatStr2
	sub rsp, 8
	;declaring formatStr3
	sub rsp, 8
	;declaring formatStr4
	sub rsp, 8
	;declaring temp
	sub rsp, 8
	;assigning formatStr1
	mov rax, 27338748441423429
	mov [rbp-24], rax
	;assigning formatStr2
	mov rax, 32199620762627616
	mov [rbp-32], rax
	;assigning formatStr3
	mov rax, 8250
	mov [rbp-40], rax
	;assigning temp
	;preparing to call printf
	;preparing arguments
	;reference operator (&)
	lea rcx, [rbp-24]
	mov rdx, 0
	;preparing stack
	push rbx
	mov rbx, rsp
	and rsp, -16
	sub rsp, 32
	call printf
	mov rsp, rbx
	pop rbx
	mov [rbp-56], rax
	;assigning temp
	;preparing to call printf
	;preparing arguments
	;reference operator (&)
	lea rcx, [rbp-32]
	mov rdx, 0
	;preparing stack
	push rbx
	mov rbx, rsp
	and rsp, -16
	sub rsp, 32
	call printf
	mov rsp, rbx
	pop rbx
	mov [rbp-56], rax
	;assigning temp
	;preparing to call printf
	;preparing arguments
	;reference operator (&)
	lea rcx, [rbp-40]
	mov rdx, 0
	;preparing stack
	push rbx
	mov rbx, rsp
	and rsp, -16
	sub rsp, 32
	call printf
	mov rsp, rbx
	pop rbx
	mov [rbp-56], rax
	;assigning formatStr4
	mov rax, 1684827173
	mov [rbp-48], rax
	;assigning temp
	;preparing to call scanf
	;preparing arguments
	;reference operator (&)
	lea rax, [rbp-48]
	push rax
	;reference operator (&)
	lea rax, [rbp-8]
	mov rdx, rax
	pop rcx
	;preparing stack
	push rbx
	mov rbx, rsp
	and rsp, -16
	sub rsp, 32
	call scanf
	mov rsp, rbx
	pop rbx
	mov [rbp-56], rax
	;assigning result
	;preparing to call fib
	push qword [rbp-8]
	call fib
	;reclaim stack space
	add rsp, 8
	mov [rbp-16], rax
	;assigning formatStr1
	mov rax, 27973149637372230
	mov [rbp-24], rax
	;assigning formatStr2
	mov rax, 35624323148131
	mov [rbp-32], rax
	;assigning formatStr3
	mov rax, 544434464
	mov [rbp-40], rax
	;assigning temp
	;preparing to call printf
	;preparing arguments
	;reference operator (&)
	lea rcx, [rbp-24]
	mov rdx, 0
	;preparing stack
	push rbx
	mov rbx, rsp
	and rsp, -16
	sub rsp, 32
	call printf
	mov rsp, rbx
	pop rbx
	mov [rbp-56], rax
	;assigning temp
	;preparing to call printf
	;preparing arguments
	;reference operator (&)
	lea rcx, [rbp-32]
	mov rdx, 0
	;preparing stack
	push rbx
	mov rbx, rsp
	and rsp, -16
	sub rsp, 32
	call printf
	mov rsp, rbx
	pop rbx
	mov [rbp-56], rax
	;assigning temp
	;preparing to call printf
	;preparing arguments
	;reference operator (&)
	lea rcx, [rbp-48]
	mov rdx, [rbp-8]
	;preparing stack
	push rbx
	mov rbx, rsp
	and rsp, -16
	sub rsp, 32
	call printf
	mov rsp, rbx
	pop rbx
	mov [rbp-56], rax
	;assigning temp
	;preparing to call printf
	;preparing arguments
	;reference operator (&)
	lea rcx, [rbp-40]
	mov rdx, 0
	;preparing stack
	push rbx
	mov rbx, rsp
	and rsp, -16
	sub rsp, 32
	call printf
	mov rsp, rbx
	pop rbx
	mov [rbp-56], rax
	;assigning temp
	;preparing to call printf
	;preparing arguments
	;reference operator (&)
	lea rcx, [rbp-48]
	mov rdx, [rbp-16]
	;preparing stack
	push rbx
	mov rbx, rsp
	and rsp, -16
	sub rsp, 32
	call printf
	mov rsp, rbx
	pop rbx
	mov [rbp-56], rax
	;get return value
	mov rax, 0
	;restore stack
	mov rsp, rbp
	pop rbp
	ret
