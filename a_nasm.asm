[BITS 32]

    GLOBAL  api_putchar
    GLOBAL  api_end
    GLOBAL  api_putstr0
    GLOBAL  api_openwin
    GLOBAL  api_putstrwin
    GLOBAL  api_boxfillwin
    GLOBAL  api_initmalloc
    GLOBAL  api_malloc
    GLOBAL  api_free
    GLOBAL  api_point
    GLOBAL  api_refreshwin
    GLOBAL  api_linewin
    GLOBAL  api_closewin
    GLOBAL  api_getkey
    GLOBAL  api_alloctimer
    GLOBAL  api_inittimer
    GLOBAL  api_settimer
    GLOBAL  api_freetimer
    GLOBAL  api_beep

[SECTION .text]

api_putchar:
    MOV   EDX,1
    MOV   AL,[ESP+4]
    INT   0x40
    RET

api_end:
    MOV  EDX,4
    INT  0x40

api_putstr0:
    PUSH  EBX
    MOV   EDX,2
    MOV   EBX,[ESP+8]
    INT   0x40
    POP   EBX
    RET

api_openwin:
    PUSH  EDI
    PUSH  ESI
    PUSH  EBX
    MOV   EDX,5
    MOV   EBX,[ESP+16]
    MOV   ESI,[ESP+20]
    MOV   EDI,[ESP+24]
    MOV   EAX,[ESP+28]
    MOV   ECX,[ESP+32]
    INT   0x40
    POP   EBX
    POP   ESI
    POP   EDI
    RET

api_putstrwin:
    PUSH  EDI
    PUSH  ESI
    PUSH  EBP
    PUSH  EBX
    MOV   EDX,6
    MOV   EBX,[ESP+20]
    MOV   ESI,[ESP+24]
    MOV   EDI,[ESP+28]
    MOV   EAX,[ESP+32]
    MOV   ECX,[ESP+36]
    MOV   EBP,[ESP+40]
    INT   0x40
    POP   EBX
    POP   EBP
    POP   ESI
    POP   EDI
    RET

api_boxfillwin:
    PUSH  EDI
    PUSH  ESI
    PUSH  EBP
    PUSH  EBX
    MOV   EDX,7
    MOV   EBX,[ESP+20]
    MOV   EAX,[ESP+24]
    MOV   ECX,[ESP+28]
    MOV   ESI,[ESP+32]
    MOV   EDI,[ESP+36]
    MOV   EBP,[ESP+40]
    INT   0x40
    POP   EBX
    POP   EBP
    POP   ESI
    POP   EDI
    RET

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

api_malloc:
    PUSH  EBX
    MOV   EDX,9
    MOV   EBX,[CS:0x0020]
    MOV   ECX,[ESP+8]
    INT   0x40
    POP   EBX
    RET

api_free:
    PUSh  EBX
    MOV   EDX,10
    MOV   BX,[CS:0x0020]
    MOV   EAX,[ESP+8]
    MOV   ECX,[ESP+12]
    INT   0x40
    POP   EBX
    RET

api_point:
    PUSH  EDI
    PUSH  ESI
    PUSH  EBX
    MOV   EDX,11
    MOV   EBX,[ESP+16]
    MOV   ESI,[ESP+20]
    MOV   EDI,[ESP+24]
    MOV   EAX,[ESP+28]
    INT   0x40
    POP   EBX
    POP   ESI
    POP   EDI
    RET

api_refreshwin:
    PUSH  EDI
    PUSH  ESI
    PUSH  EBX
    MOV   EDX,12
    MOV   EBX,[ESP+16]
    MOV   EAX,[ESP+20]
    MOV   ECX,[ESP+24]
    MOV   ESI,[ESP+28]
    MOV   EDI,[ESP+32]
    INT   0x40
    POP   EBX
    POP   ESI
    POP   EDI
    RET

api_linewin:
    PUSH  EDI
    PUSH  ESI
    PUSH  EBP
    PUSH  EBX
    MOV   EDX,13
    MOV   EBX,[ESP+20]
    MOV   EAX,[ESP+24]
    MOV   ECX,[ESP+28]
    MOV   ESI,[ESP+32]
    MOV   EDI,[ESP+36]
    MOV   EBP,[ESP+40]
    INT   0x40
    POP   EBX
    POP   EBP
    POP   ESI
    POP   EDI
    RET

api_closewin:
    PUSH  EBX
    MOV   EDX,14
    MOV   EBX,[ESP+8]
    INT   0x40
    POP   EBX
    RET

api_getkey:
    MOV   EDX,15
    MOV   EAX,[ESP+4]
    INT   0x40
    RET

api_alloctimer:
    MOV   EDX,16
    INT   0x40
    RET

api_inittimer:
    PUSH  EBX
    MOV   EDX,17
    MOV   EBX,[ESP+8]
    MOV   EAX,[ESP+12]
    INT   0x40
    POP   EBX
    RET

api_settimer:
    PUSH  EBX
    MOV   EDX,18
    MOV   EBX,[ESP+8]
    MOV   EAX,[ESP+12]
    INT   0x40
    POP   EBX
    RET

api_freetimer:
    PUSH  EBX
    MOV   EDX,19
    MOV   EBX,[ESP+8]
    INT   0x40
    POP   EBX
    RET

api_beep:
    MOV   EDX,20
    MOV   EAX,[ESP+4]
    INT   0x40
    RET
