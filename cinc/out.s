.globl _main
_main:
push rbp
mov rbp,rsp
mov rax,0
push rax
mov rax,0
push rax
l0:
mov rax,[rbp-16]
push rax
mov rax,7
pop rcx
cmp rcx,rax
mov rax,0
setl al
je l1
mov rax,[rbp-8]
push rax
mov rax,2
pop rcx
add rax,rcx
mov [rbp-8],rax
add rsp,0
l2:
mov rax,[rbp-16]
inc rax
mov [rbp-16],rax
dec rax
jmp l0
l1:
add rsp,8
mov rax,[rbp-8]
mov rsp,rbp
pop rbp
ret
add rsp,8
mov rax,0
mov rsp,rbp
pop rbp
ret
