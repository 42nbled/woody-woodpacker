section .text

global __start_payload
global __stop_payload
global _encrypted_start
global _encrypted_end

extern	_start

__start_payload:
	push	rdx

	mov		rax, 9
	mov		rdi, 0
	mov		rsi, 4096
	mov		rdx, 7		; PROT_READ | PROT_WRITE | PROT_EXEC
	mov		r10, 0x22	; MAP_PRIVATE | MAP_ANONYMOUS
	mov		r8, 0
	mov		r9, 0
	syscall

	push	rax
	lea		rbx, [rel _encrypted_start]
	lea		rdx, [rel _encrypted_end]
.loop_start:
	cmp 	rbx, rdx
	je		.loop_end
	mov		cl, byte [rbx]
	xor		cl, 0x42
	mov		byte [rax], cl
	inc		rax
	inc		rbx
	jmp		.loop_start
.loop_end:
	pop		rax
	call	rax

	pop		rdx
	jmp		_start
_encrypted_start:
	lea		rsi,[rel msg]
	mov		rax, 1
	mov		rdi, 1
	mov		rdx, 14
	syscall
	ret
msg:
	db "....WOODY....",10
_encrypted_end:
__stop_payload:
