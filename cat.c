#include"apifunc.h"
void HariMain(){
  int fh;
  char c,cmdline[30],*p;
  api_cmdline(cmdline,30);
  p = cmdline;
  while(*p!=' ')p++;
  while(*p==' ')p++;
  fh = api_fopen(p);
  if(fh!=0){
    while(1){
      if(api_fread(&c,1,fh)==0){
        break;
      }
      api_putchar(c);
    }
  }else{
    api_putstr0("File Not Found\n");
  }
  api_end();
}
