[BITS 32]

    GLOBAL  api_fseek

[SECTION .text]

api_fseek:
    PUSH  EBX
    MOV   EDX,23
    MOV   EAX,[ESP+8]
    MOV   ECX,[ESP+16]
    MOV   EBX,[ESP+12]
    INT   0x40
    POP   EBX
    RET
