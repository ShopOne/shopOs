[BITS 32]

    GLOBAL  api_fread

[SECTION .text]

api_fread:
  PUSH  EBX
  MOV   EDX,25
  MOV   EAX,[ESP+16]
  MOV   ECX,[ESP+12]
  MOV   EBX,[ESP+8]
  INT   0x40
  POP   EBX
  RET  
