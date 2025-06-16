section .text

global __start_payload
global __stop_payload
global _encrypted_start
global _encrypted_end
global key_start

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
	mov		rsi, rax ; dst

	lea		rdi, [rel _encrypted_start] ; src
	lea		rcx, [rel _encrypted_end]
	sub		rcx, rdi 				; len
	lea		rdx, [rel key_start] 			; key

	mov		r8, __stop_payload - key_start ; L



	push    r12
	xor     eax, eax
	mov     r9, rdi
	mov     r10, rsi
	push    rbp
	mov     rsi, rcx
	mov     ecx, 64
	push    rbx
	mov     rbx, rdx
	sub     rsp, 136
	lea     rdi, [rsp-120]
	rep stosd
	xor     eax, eax
	lea     rcx, [rsp-120]
.L2:
	mov     BYTE [rcx+rax], al
	inc     rax
	cmp     rax, 256
	jne     .L2
	xor     r11d, r11d
	xor     r12d, r12d
	mov     ebp, 256
.L3:
	mov     rax, r11
	xor     edx, edx
	mov     dil, BYTE [rcx]
	inc     r11
	div     r8
	movzx   eax, BYTE [rbx+rdx]
	movzx   edx, dil
	add     edx, r12d
	add     eax, edx
	cdq
	idiv    ebp
	movsx   rax, edx
	xor     dil, BYTE [rsp-120+rax]
	mov     r12, rax
	mov     BYTE [rcx], dil
	xor     dil, BYTE [rsp-120+rax]
	mov     BYTE [rsp-120+rax], dil
	xor     BYTE [rcx], dil
	inc     rcx
	cmp     r11, 256
	jne     .L3
	xor     edi, edi
	xor     ecx, ecx
	xor     edx, edx
.L4:
	cmp     rdi, rsi
	je      .L10
	inc     rdx
	movzx   edx, dl
	mov     al, BYTE [rsp-120+rdx]
	add     ecx, eax
	movzx   ecx, cl
	xor     al, BYTE [rsp-120+rcx]
	mov     BYTE [rsp-120+rdx], al
	xor     al, BYTE [rsp-120+rcx]
	mov     BYTE [rsp-120+rcx], al
	xor     al, BYTE [rsp-120+rdx]
	mov     BYTE [rsp-120+rdx], al
	add     al, BYTE [rsp-120+rcx]
	movzx   eax, al
	mov     al, BYTE [rsp-120+rax]
	xor     al, BYTE [r9+rdi]
	mov     BYTE [r10+rdi], al
	inc     rdi
	jmp     .L4
.L10:
	add     rsp, 136
	pop     rbx
	pop     rbp
	pop     r12





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
key_start:
	times 64 db 0
__stop_payload:
