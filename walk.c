#include"apifunc.h"
void HariMain(){
  char *buf;
  int win,info,x,y;
  api_initmalloc();
  buf = api_malloc(160*100);
  win = api_openwin(buf,160,100,-1,"walk");
  api_boxfillwin(win,4,24,155,95,0);
  x = 76;
  y = 56;
  api_putstrwin(win,x,y,3,1,"@");
  while(1){
    info = api_getkey(1);
    api_putstrwin(win,x,y,0,1,"@");
    if(info=='4'&&x>4) x-=8;
    if(info=='6'&&x<148) x+=8;
    if(info=='8'&&y>24) y-=8;
    if(info=='2'&&y<80) y+=8;
    api_putstrwin(win,x,y,3,1,"@");
    if(info==0x0a) break;
  }
  api_closewin(win);
  api_end();
}
