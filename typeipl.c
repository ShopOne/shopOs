#include"apifunc.h"
void HariMain(){
  int fh;
  char c;
  fh = api_fopen("Makefile");
  if(fh!=0){
    while(1){
      if(api_fread(&c,1,fh)==0){
        break;
      }
      api_putchar(c);
    }
  }
  api_end();
}
