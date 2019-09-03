#include"bootpack.h"
void enable_mouse(FIFO32 *fifo,int data0,MOUSE_DEC *mdec){
  mouse_fifo = fifo;
  mousedata0 = data0;
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD,KEYCMD_SENDTO_MOUSE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT,MOUSECMD_ENABLE);
  mdec->phase=0;
  return;
}
void inthandler2c(int *esp){
  int data;
  io_out8(PIC1_OCW2,0x64);
  io_out8(PIC0_OCW2,0x62);
  data = io_in8(PORT_KEYDAT);
  fifo32_put(mouse_fifo,data+mousedata0);
  return;
}
int mouse_decode(MOUSE_DEC *mdec,unsigned char dat){
  if(mdec->phase==0){
    if(dat==0xfa){
      mdec->phase = 1;
      return 0;
    }
  }else{
    if(mdec->phase==3){
      mdec->buf[2]=dat;
      mdec->phase=1;
      mdec->btn=mdec->buf[0]&0x07;
      mdec->x=mdec->buf[1];
      mdec->y=mdec->buf[2];
      if((mdec->buf[0]&0x10)!=0){
        mdec->x |= 0xffffff00;
      }
      if((mdec->buf[0]&0x20)!=0){
        mdec->y |= 0xffffff00;
      }
      mdec->y = -mdec->y;
      return 1;
    }else if(mdec->phase==1){
      if((dat&0xc8)==0x08){
        mdec->phase++;
        mdec->buf[0]=dat;
        return 0;
      }
    }else if(mdec->phase==2){
      mdec->buf[1]=dat;
      mdec->phase++;
      return 0;
    }
  }
  return -1;
}
