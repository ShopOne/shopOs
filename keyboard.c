#include"bootpack.h"
void inthandler21(int *esp){
  int data;
  io_out8(PIC0_OCW2,0x61);
  data = io_in8(PORT_KEYDAT);
  fifo32_put(key_fifo,data+keydata0);
  return;
}
void wait_KBC_sendready(){
  while(1){
    if((io_in8(PORT_KEYSTA)&KEYSTA_SEND_NOTREADY)==0){
      break;
    }
  }
  return;
}
void init_keyboard(FIFO32 *fifo, int data0){
  key_fifo=fifo;
  keydata0=data0;
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD,KEYCMD_WRITE_MODE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT,KBC_MODE);
  return;
}
