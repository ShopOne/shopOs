#include"apifunc.h"
void HariMain(){
  int win;
  char buf[150*70];
  win =api_openwin(buf,150,70,255,"notrec");
  api_boxfillwin(win,0,50,34,69,255);
  api_boxfillwin(win,115,50,149,69,255);
  api_boxfillwin(win,50,30,99,49,255);
  while(1){
  api_refreshwin(win,0,0,150,70);
    if(api_getkey(1)==0x0a){
      break;
    }
  }
  api_end();

}
