#include"apifunc.h"
void HariMain(){
  char *buf;
  int win,info;
  api_initmalloc();
  buf = api_malloc(160*100);
  win = api_openwin(buf,160,100,-1,"lines");
  for(int i=0;i<6;i++){
    api_linewin(win+1,8,26,77,i*9+26,i);
    api_linewin(win+1,88,26,i*9+88,89,i);
  }
  api_refreshwin(win,6,26,154,90);
  while(1){
    info = api_getkey(1);
    if(info==0x0a){
      break;
    }
    if(info==1){
      api_putchar('a');
    }
  }
  api_closewin(win);
  api_end();
}
