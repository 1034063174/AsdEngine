

     .code
    
     EXTERN OrgSyscall:PROC  ;              ; �����ⲿ���� abc
     EXTERN CommFunction:PROC ;              ; �����ⲿ����  ����

     int 3;
    CommShellcode PROC

        push rax            ; ���� RAX
    push rbx            ; ���� RBX
    push rcx            ; ���� RCX
    push rdx            ; ���� RDX
    push rsi            ; ���� RSI
    push rdi            ; ���� RDI
    push r8             ; ���� R8
    push r9             ; ���� R9
    push r10            ; ���� R10
    push r11            ; ���� R11
    push r12            ; ���� R12
    push r13            ; ���� R13
    push r14            ; ���� R14
    push r15            ; ���� R15



    sub rsp  ,100h;   ����Ŀ�꺯��
    call  CommFunction;            
    add rsp  ,100h;

    ; ��ԭ�Ĵ���

    pop r15             ; ��ԭ R15
    pop r14             ; ��ԭ R14
    pop r13             ; ��ԭ R13
    pop r12             ; ��ԭ R12
    pop r11             ; ��ԭ R11
    pop r10             ; ��ԭ R10
    pop r9              ; ��ԭ R9
    pop r8              ; ��ԭ R8
    pop rdi             ; ��ԭ RDI
    pop rsi             ; ��ԭ RSI
    pop rdx             ; ��ԭ RDX
    pop rcx             ; ��ԭ RCX
    pop rbx             ; ��ԭ RBX
    pop rax             ; ��ԭ RAX


    mov rax, OrgSyscall   ;����
    mov rax,[rax]
    call rax
    ret ;
    CommShellcode ENDP
     END