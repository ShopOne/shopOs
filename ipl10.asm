; haribote-ipl
; TAB=4
;   MOV A, B   AにBを代入
;   DB        メモリに書き込み 1byte
;   RESB      メモリをA開ける
;   DW        data word ,16byte書く
;   DD        data double word 倍なので32
;   JMP       ジャンプ命令、そのメモリへ強制移動
;   ORG       origin,どこからスタートするか決める
;   CMP A, B  AとB比較して…
;   JE        CMPがtrueならジャンプ
;   INT       割り込んでなんか動かす
;   AX アキュームレータ、累積演算器
;   CX カウンタ
;   DX データ
;   BX ベース
;   SP スタックポインタ
;   BP ベースポインタ
;   SI ソース(よみこみ)インデックス
;   DI デスティネーション(書き込み)インデックス

CYLS    EQU     12

        ORG     0x7c00          ; このプログラムがどこに読み込まれるか

;FAT12フォーマットフロッピーのための記述
        JMP     entry
        DB      0x90
        DB      "Haribote"      ; ブートセレクタの名前を自由に書いていい
        DW      512             ; 1セクタの大きさ(512必須)
        DB      1               ; クラスタの大きさ、1必須
        DW      1               ; FATのスタート位置、普通1

    ;main program


; プログラム本体

entry:
    MOV    AX,0      ; レジスタ初期化
    MOV    SS,AX
    MOV    SP,0x7c00
    MOV    DS,AX

; ディスクを読む

    MOV    AX,0x0820
    MOV    ES,AX
    MOV    CH,0      ; シリンダ0
    MOV    DH,0      ; ヘッド0
    MOV    CL,2      ; セクタ2
    MOV    BX,18*2*CYLS-1  ; 読み込みたい合計セクタ数
    CALL  readfast    ; 高速読み込み

; 読み終わったのでharibote.sysを実行だ！

    MOV    BYTE [0x0ff0],CYLS  ; IPLがどこまで読んだのかをメモ
    JMP    0xa400

error:
    MOV    AX,0
    MOV    ES,AX
    MOV    SI,msg
putloop:
    MOV    AL,[SI]
    ADD    SI,1      ; SIに1を足す
    CMP    AL,0
    JE    fin
    MOV    AH,0x0e      ; 一文字表示ファンクション
    MOV    BX,15      ; カラーコード
    INT    0x10      ; ビデオBIOS呼び出し
    JMP    putloop
fin:
    HLT            ; 何かあるまでCPUを停止させる
    JMP    fin        ; 無限ループ
msg:
    DB    0x0a, 0x0a    ; 改行を2つ
    DB    "load error"
    DB    0x0a      ; 改行
    DB    0

readfast:  ; ALを使ってできるだけまとめて読み出す
;  ES:読み込み番地, CH:シリンダ, DH:ヘッド, CL:セクタ, BX:読み込みセクタ数

    MOV    AX,ES      ; < ESからALの最大値を計算 >
    SHL    AX,3      ; AXを32で割って、その結果をAHに入れたことになる （SHLは左シフト命令）
    AND    AH,0x7f      ; AHはAHを128で割った余り（512*128=64K）
    MOV    AL,128      ; AL = 128 - AH; 一番近い64KB境界まで最大何セクタ入るか
    SUB    AL,AH

    MOV    AH,BL      ; < BXからALの最大値をAHに計算 >
    CMP    BH,0      ; if (BH != 0) { AH = 18; }
    JE    .skip1
    MOV    AH,18
.skip1:
    CMP    AL,AH      ; if (AL > AH) { AL = AH; }
    JBE    .skip2
    MOV    AL,AH
.skip2:

    MOV    AH,19      ; < CLからALの最大値をAHに計算 >
    SUB    AH,CL      ; AH = 19 - CL;
    CMP    AL,AH      ; if (AL > AH) { AL = AH; }
    JBE    .skip3
    MOV    AL,AH
.skip3:

    PUSH  BX
    MOV    SI,0      ; 失敗回数を数えるレジスタ
retry:
    MOV    AH,0x02      ; AH=0x02 : ディスク読み込み
    MOV    BX,0
    MOV    DL,0x00      ; Aドライブ
    PUSH  ES
    PUSH  DX
    PUSH  CX
    PUSH  AX
    INT    0x13      ; ディスクBIOS呼び出し
    JNC    next      ; エラーがおきなければnextへ
    ADD    SI,1      ; SIに1を足す
    CMP    SI,5      ; SIと5を比較
    JAE    error      ; SI >= 5 だったらerrorへ
    MOV    AH,0x00
    MOV    DL,0x00      ; Aドライブ
    INT    0x13      ; ドライブのリセット
    POP    AX
    POP    CX
    POP    DX
    POP    ES
    JMP    retry
next:
    POP    AX
    POP    CX
    POP    DX
    POP    BX        ; ESの内容をBXで受け取る
    SHR    BX,5      ; BXを16バイト単位から512バイト単位へ
    MOV    AH,0
    ADD    BX,AX      ; BX += AL;
    SHL    BX,5      ; BXを512バイト単位から16バイト単位へ
    MOV    ES,BX      ; これで ES += AL * 0x20; になる
    POP    BX
    SUB    BX,AX
    JZ    .ret
    ADD    CL,AL      ; CLにALを足す
    CMP    CL,18      ; CLと18を比較
    JBE    readfast    ; CL <= 18 だったらreadfastへ
    MOV    CL,1
    ADD    DH,1
    CMP    DH,2
    JB    readfast    ; DH < 2 だったらreadfastへ
    MOV    DH,0
    ADD    CH,1
    JMP    readfast
.ret:
    RET

    TIMES  0x1fe-($-$$) DB 0    ; 0x7dfeまでを0x00で埋める命令

    DB    0x55, 0xaa
