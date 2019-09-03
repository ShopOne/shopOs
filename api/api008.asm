[BITS 32]

    GLOBAL  api_initmalloc

[SECTION .text]
api_initmalloc:
    PUSH  EBX
    MOV   EDX,8
    MOV   EBX,[CS:0x0020]
    MOV   EAX,EBX
    ADD   EAX,32*1024
    MOV   EAX,[CS:0x0000]
    SUB   ECX,EAX
    INT   0x40
    POP   EBX
    RET
