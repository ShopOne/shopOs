section .text
    GLOBAL  io_hlt
    GLOBAL  io_cli
    GLOBAL  io_sti
    GLOBAL  io_stihlt
    GLOBAL  io_in8
    GLOBAL  io_in16
    GLOBAL  io_in32 
    GLOBAL  io_out8 
    GLOBAL  io_out16
    GLOBAL  io_out32
    GLOBAL  io_load_eflags
    GLOBAL  io_store_eflags
    GLOBAL  load_gdtr
    GLOBAL  load_idtr
    GLOBAL  asm_inthandler20
    GLOBAL  asm_inthandler21
    GLOBAL  asm_inthandler27
    GLOBAL  asm_inthandler2c
    GLOBAL  asm_inthandler0d
    GLOBAL  asm_inthandler0c
    GLOBAL  asm_cons_putchar
    GLOBAL  asm_end_app
    GLOBAL  load_cr0
    GLOBAL  store_cr0
    GLOBAL  memtest_sub
    GLOBAL  load_tr
    GLOBAL  taskswitch4
    GLOBAL  taskswitch3
    GLOBAL  farjmp
    GLOBAL  farcall
    GLOBAl  asm_hrb_api
    GLOBAL  start_app
    EXTERN  inthandler21
    EXTERN  inthandler27
    EXTERN  inthandler2c
    EXTERN  inthandler20
    EXTERN  inthandler0d
    EXTERN  inthandler0c
    EXTERN  cons_putchar
    EXTERN  hrb_api

io_hlt:          ; void io_hlt();
        HLT
        RET 
io_cli:
        CLI
        RET

io_sti:
        STI
        RET

io_stihlt:
        STI
        HLT
        RET

io_in8:   ;int io_in8(int port)
        MOV     EDX, [ESP+4] ;port
        MOV     EAX, 0
        IN      AL, DX
        RET

io_in16:   ;int io_in8(int port)
        MOV     EDX, [ESP+4] ;port
        MOV     EAX, 0
        IN      AL, DX
        RET

io_in32:   ;int io_in8(int port)
        MOV     EDX, [ESP+4] ;port
        MOV     EAX, 0
        IN      AL, DX
        RET
       
io_out8:   ;int io_out8(int port, int data)
        MOV     EDX, [ESP+4]
        MOV     AL, [ESP+8]
        OUT     DX, AL
        RET

io_out16:   ;int io_out16(int port, int data)
        MOV     EDX, [ESP+4]
        MOV     AL, [ESP+8]
        OUT     DX, AX
        RET
io_out32:   ;int io_out32(int port, int data)
        MOV     EDX, [ESP+4]
        MOV     AL, [ESP+8] 
        OUT     DX, EAX 
        RET

io_load_eflags:   
        PUSHFD      ; PUSH EFLAGS
        POP     EAX
        RET

io_store_eflags:
        MOV     EAX, [ESP+4]
        PUSH    EAX
        POPFD       ; pop EFLAGS
        RET

load_gdtr: ; void load_gdtr(int limit ,int addr)
        MOV         AX, [ESP+4]
        MOV         [ESP+6], AX
        LGDT        [ESP+6]
        RET

load_idtr: ; void load_idtr(int limit ,int addr)
        MOV         AX, [ESP+4]
        MOV         [ESP+6], AX
        LIDT        [ESP+6]
        RET

asm_inthandler20:
    PUSH  ES
    PUSH  DS
    PUSHAD
    MOV    EAX,ESP
    PUSH  EAX
    MOV    AX,SS
    MOV    DS,AX
    MOV    ES,AX
    CALL  inthandler20
    POP    EAX
    POPAD
    POP    DS
    POP    ES
    IRETD

asm_inthandler21:
    PUSH  ES
    PUSH  DS
    PUSHAD
    MOV    EAX,ESP
    PUSH  EAX
    MOV    AX,SS
    MOV    DS,AX
    MOV    ES,AX
    CALL  inthandler21
    POP    EAX
    POPAD
    POP    DS
    POP    ES
    IRETD

asm_inthandler27:
    PUSH  ES
    PUSH  DS
    PUSHAD
    MOV    EAX,ESP
    PUSH  EAX
    MOV    AX,SS
    MOV    DS,AX
    MOV    ES,AX
    CALL  inthandler27
    POP    EAX
    POPAD
    POP    DS
    POP    ES
    IRETD

asm_inthandler2c:
    PUSH  ES
    PUSH  DS
    PUSHAD
    MOV    EAX,ESP
    PUSH  EAX
    MOV    AX,SS
    MOV    DS,AX
    MOV    ES,AX
    CALL  inthandler2c
    POP    EAX
    POPAD
    POP    DS
    POP    ES
    IRETD

asm_inthandler0d:
    STI
    PUSH  ES
    PUSH  DS
    PUSHAD
    MOV    EAX,ESP
    PUSH  EAX
    MOV    AX,SS
    MOV    DS,AX
    MOV    ES,AX
    CALL  inthandler0d
    CMP    EAX,0    ; ここだけ違う
    JNE    asm_end_app    ; ここだけ違う
    POP    EAX
    POPAD
    POP    DS
    POP    ES
    ADD    ESP,4      ; INT 0x0d では、これが必要
    IRETD

asm_inthandler0c:
    STI
    PUSH    ES 
    PUSH    DS
    PUSHAD  
    MOV     EAX,ESP
    PUSH    EAX
    MOV     AX,SS
    MOV     DS,AX
    MOV     ES,AX
    CALL    inthandler0c
    CMP     EAX,0
    JNE     asm_end_app
    POP     EAX
    POPAD
    POP     DS
    POP     ES 
    ADD     ESP,4
    IRETD

load_cr0:
    MOV   EAX, CR0
    RET

store_cr0:
    MOV   EAX, [ESP+4]
    MOV   CR0, EAX

memtest_sub: ; (unsigned int start,unsigned int end)
    PUSH  EDI
    PUSH  ESI
    PUSH  EBX
    MOV   ESI, 0xaa55aa55
    MOV   EDI, 0x55aa55aa
    MOV   EAX, [ESP+12+4]

mts_loop:
    MOV   EBX, EAX 
    ADD   EBX, 0xffc ; p= i+0xffc  
    MOV   EDX, [EBX] ; old = *p
    MOV   [EBX], ESI ; *p= pat0
    XOR   DWORD [EBX] , 0xffffffff ; *p ^= 0xffffffff
    CMP   EDI, [EBX] ; *p!=pat1 goto fin
    JNE   mts_fin
    XOR   DWORD [EBX] , 0xffffffff
    CMP   ESI, [EBX] ; *p!=pat0 goto fin
    JNE   mts_fin
    MOV   [EBX], EDX
    ADD   EAX, 0x1000
    CMP   EAX, [ESP+12+8]
    JBE   mts_loop
    POP   EBX
    POP   ESI
    POP   EDI
    RET
mts_fin:
    MOV   [EBX], EDX
    POP   EBX
    POP   ESI
    POP   EDI
    RET

load_tr:
    LTR   [ESP+4]
    RET

taskswitch4:
    JMP   4*8:0
    RET

taskswitch3:
    JMP   3*8:0
    RET

farjmp:
    JMP  FAR [ESP+4]
    RET
farcall:
    CALL FAR [ESP+4]
    RET

asm_cons_putchar:
    STI
    PUSHAD
    PUSH  1
    AND   EAX, 0xff
    PUSH  EAX
    PUSH  DWORD [0x0fec]
    CALL  cons_putchar
    ADD   ESP,12
    POPAD
    IRETD

asm_hrb_api:
    STI
    PUSH  DS
    PUSH  ES
    PUSHAD    ; 保存のためのPUSH
    PUSHAD
    MOV    AX,SS
    MOV    DS,AX      ; とりあえずDSだけOS用にする
    MOV    ES,AX
    CALL   hrb_api
    CMP    EAX,0
    JNE    asm_end_app
    ADD    ESP,32
    POPAD
    POP    ES
    POP    DS
    IRETD    ; この命令が自動でSTIしてくれる

asm_end_app:
    MOV   ESP,[EAX]
    MOV   DWORD [EAX+4],0
    POPAD  
    RET
    
start_app:    ; void start_app(int eip, int cs, int esp, int ds);
    PUSHAD    ; 32ビットレジスタを全部保存しておく
    MOV    EAX,[ESP+36]  ; アプリ用のEIP
    MOV    ECX,[ESP+40]  ; アプリ用のCS
    MOV    EDX,[ESP+44]  ; アプリ用のESP
    MOV    EBX,[ESP+48]  ; アプリ用のDS/SS
    MOV    EBP,[ESP+52]
    MOV    [EBP  ],ESP    ; OS用のESP
    MOV    [EBP+4],SS
    MOV    ES,BX
    MOV    DS,BX
    MOV    FS,BX
    MOV    GS,BX

    OR        ECX,3
    OR        EBX,3
    PUSH    EBX
    PUSH    EDX
    PUSH    ECX
    PUSH    EAX
    RETF

