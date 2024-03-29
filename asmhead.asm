; haribote os!!!!!
; boot info
VBEMODE EQU       0x100
;一覧
;0x100 640*480
;0x101 640*480
;0x103 800*600
;0x105 1024*768
;0x107 1280*1024
BOTPAK  EQU       0x00280000
DSKCAC  EQU       0x00100000
DSKCAC0 EQU       0x00008000

CYLS    EQU       0x0ff0 ; ブートセクタが設定する
LEDS    EQU       0x0ff1 
VMODE   EQU       0x0ff2 ; 色数に関する情報、何ビットカラー？
SCRNX   EQU       0x0ff4 ; 解像度x
SCRNY   EQU       0x0ff6 ; 解像度y
VRAM    EQU       0x0ff8 ; グラフィックバッファの開始位置

        ORG       0xa400   ; このプログラムがどこに読み込まれるのか

; VBE確認
        MOV       AX, 0x9000
        MOV       ES, AX
        MOV       DI, 0
        MOV       AX, 0x4f00
        INT       0x10
        CMP       AX, 0x004f
        JNE       scrn320

; バージョンチェック
        MOV       AX, [ES:DI+4]
        CMP       AX, 0x0200
        JB        scrn320

;画面モード情報
        MOV       CX, VBEMODE
        MOV       AX, 0x4f01
        INT       0x10
        CMP       AX, 0x004f
        JNE       scrn320

;情報確認
        CMP       BYTE [ES:DI+0x19], 8
        JNE       scrn320
        CMP       BYTE [ES:DI+0x1b], 4
        JNE       scrn320
        MOV       AX, [ES:DI+0x00]
        AND       AX, 0x0080
        JZ        scrn320              ;bit7なのであきらめ

;きりかえ
        MOV       BX, VBEMODE+0x4000
        MOV       AX, 0x4f02
        INT       0x10
        MOV       BYTE [VMODE], 8
        MOV       AX, [ES:DI+0x12]
        MOV       [SCRNX], AX
        MOV       AX, [ES:DI+0x14]
        MOV       [SCRNY], AX
        MOV       EAX, [ES:DI+0x28]
        MOV       [VRAM], EAX
        JMP       keystatus

scrn320:                    ; あきらめ
        MOV       AL, 0x13
        MOV       AH, 0x00
        INT       0x10
        MOV       BYTE [VMODE], 8 ; 画面をメモする
        MOV       WORD [SCRNX], 320
        MOV       WORD [SCRNY], 200
        MOV       DWORD [VRAM], 0x000a0000

; キーボードのLED状態をBIOSに聞く

keystatus:
        MOV       AH, 0x02
        INT       0x16
        MOV       [LEDS], AL
; PCIが割り込みを受け付けないように
        MOV       AL, 0xff
        OUT       0x21, AL
        NOP                 ;連続させるといけないらしい
        OUT       0xa1,AL

        CLI                 ;CPUでもだめ

;CPUから1MB以上のメモリにアクセスできるようにする
        CALL    waitkbdout
        MOV           AL, 0xd1
        OUT           0x64, AL
        CALL    waitkbdout
        MOV           AL,0xdf
        OUT           0x60, AL
        CALL    waitkbdout

; プロテクトモードへ
        LGDT    [GDTR0]               ;暫定GDTせってい
        MOV           EAX, CR0
        AND           EAX, 0x7fffffff ; bit31を0にする(ベーシング禁止のため)
        OR            EAX, 0x00000001 ; bit0を1にする(プロテクトモードへするため)
        MOV           CR0, EAX
        JMP           pipelineflush
pipelineflush:
        
        MOV           AX, 1*8         ; 読み書き可能セグメント  
        MOV           DS, AX
        MOV           ES, AX
        MOV           FS, AX
        MOV           GS, AX
        MOV           SS, AX

; bootpack転送
        MOV           ESI, bootpack ;転送元
        MOV           EDI, BOTPAK  ;転送先
        MOV           ECX, 512*1024/4
        CALL          memcpy

; ディスクデータも転送
; ブートセクタ
        MOV           ESI, 0x7c00 ;元
        MOV           EDI, DSKCAC ;先
        MOV           ECX, 512/4
        CALL          memcpy
;残りすべて

        MOV           ESI, DSKCAC0+512
        MOV           EDI, DSKCAC+512
        MOV           ECX, 0
        MOV           CL, BYTE [CYLS]
        IMUL    ECX, 512*18*2/4 ;シリンダ数からバイト数/4にする
        SUB           ECX, 512/4
        CALL          memcpy
; asmheadでしなければいけないことは全部し終わったので、
;    あとはbootpackに任せる

; bootpackの起動

        MOV        EBX,BOTPAK
        MOV        ECX,[EBX+16]
        ADD        ECX,3            ; ECX += 3;
        SHR        ECX,2            ; ECX /= 4;
        JZ        skip            ; 転送するべきものがない
        MOV        ESI,[EBX+20]    ; 転送元
        ADD        ESI,EBX
        MOV        EDI,[EBX+12]    ; 転送先
        CALL    memcpy
skip:
        MOV        ESP,[EBX+12]    ; スタック初期値
        JMP        DWORD 2*8:0x0000001b

waitkbdout:
        IN         AL,0x64
        AND         AL,0x02
        JNZ        waitkbdout        ; ANDの結果が0でなければwaitkbdoutへ
        RET

memcpy:
        MOV        EAX,[ESI]
        ADD        ESI,4
        MOV        [EDI],EAX
        ADD        EDI,4
        SUB        ECX,1
        JNZ        memcpy            ; 引き算した結果が0でなければmemcpyへ
        RET
; memcpyはアドレスサイズプリフィクスを入れ忘れなければ、ストリング命令でも書ける

        ALIGN 16, DB 0
GDT0:
        TIMES    8  DB 0            ; ヌルセレクタ
        DW        0xffff,0x0000,0x9200,0x00cf    ; 読み書き可能セグメント32bit
        DW        0xffff,0x0000,0x9a28,0x0047    ; 実行可能セグメント32bit（bootpack用）

        DW        0
GDTR0:
        DW        8*3-1
        DD        GDT0

        ALIGNB    16
bootpack:
