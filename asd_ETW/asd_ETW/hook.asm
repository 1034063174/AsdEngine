

     .code
    
     EXTERN OrgSyscall:PROC  ;              ; 声明外部变量 abc
     EXTERN CommFunction:PROC ;              ; 声明外部变量  函数

     int 3;
    CommShellcode PROC

        push rax            ; 保存 RAX
    push rbx            ; 保存 RBX
    push rcx            ; 保存 RCX
    push rdx            ; 保存 RDX
    push rsi            ; 保存 RSI
    push rdi            ; 保存 RDI
    push r8             ; 保存 R8
    push r9             ; 保存 R9
    push r10            ; 保存 R10
    push r11            ; 保存 R11
    push r12            ; 保存 R12
    push r13            ; 保存 R13
    push r14            ; 保存 R14
    push r15            ; 保存 R15



    sub rsp  ,100h;   调用目标函数
    call  CommFunction;            
    add rsp  ,100h;

    ; 还原寄存器

    pop r15             ; 还原 R15
    pop r14             ; 还原 R14
    pop r13             ; 还原 R13
    pop r12             ; 还原 R12
    pop r11             ; 还原 R11
    pop r10             ; 还原 R10
    pop r9              ; 还原 R9
    pop r8              ; 还原 R8
    pop rdi             ; 还原 RDI
    pop rsi             ; 还原 RSI
    pop rdx             ; 还原 RDX
    pop rcx             ; 还原 RCX
    pop rbx             ; 还原 RBX
    pop rax             ; 还原 RAX


    mov rax, OrgSyscall   ;跳回
    mov rax,[rax]
    call rax
    ret ;
    CommShellcode ENDP
     END