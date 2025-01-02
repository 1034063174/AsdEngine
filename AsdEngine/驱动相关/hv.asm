.code

?vmx_vmcall@hv@@YA_KAEAUhypercall_input@1@@Z proc
  ; move input into registers
  mov rax, [rcx]       ; code
  mov rdx, [rcx + 10h] ; args[1]
  mov r8,  [rcx + 18h] ; args[2]
  mov r9,  [rcx + 20h] ; args[3]
  mov r10, [rcx + 28h] ; args[4]
  mov r11, [rcx + 30h] ; args[5]
  mov rcx, [rcx + 08h] ; args[0]

  vmcall

  ret
?vmx_vmcall@hv@@YA_KAEAUhypercall_input@1@@Z endp

?vmx_hv_begin@hv@@YAPEAXPEAX0@Z proc
	mov rcx, 01ff1f1h
	vmcall
	ret
?vmx_hv_begin@hv@@YAPEAXPEAX0@Z endp

?vmx_hv_end@hv@@YAXPEAX@Z proc
	jmp rcx
	int 3
?vmx_hv_end@hv@@YAXPEAX@Z endp

?asm_vmx_excute_finish@hv@@YAXXZ proc
	
	mov rax, 010F2C16h
	mov rcx, 12345678910h
	vmcall

?asm_vmx_excute_finish@hv@@YAXXZ endp


end
