[BITS 32]

    GLOBAL  api_fclose

[SECTION .text]

api_fclose:
    MOV  EDX,22
    MOV  EAX,[ESP+4]
    INT  0x40
    RET
