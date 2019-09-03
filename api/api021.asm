[BITS 32]

    GLOBAL  api_fopen

[SECTION .text]

api_fopen:
    PUSH EBX
    MOV  EDX,21
    MOV  EBX,[ESP+8]
    INT  0x40
    POP  EBX
    RET
