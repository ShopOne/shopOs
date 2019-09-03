#include"bootpack.h"
FILEINFO *file_search(char *name,FILEINFO *finfo,int max){
  int i, j,found=1;
  char s[12];
  for(j = 0; j < 11; j++){
    s[j] = ' ';
  }
  j = 0;
  for(i = 0; name[i] != 0; i++){
    if(j >= 11){
      return 0;
    }
    if(name[i] == '.' && j <= 8){
      j = 8;
    }else{
      s[j] = name[i];
      if('a' <= s[j] && s[j] <= 'z'){
        s[j] -= 0x20;
      }
      j++;
    }
  }
  for(i = 0; i < max; ){
    if(finfo[i].name[0] == 0x00){
      break;
    }
    if((finfo[i].type & 0x18) == 0){
      found=1;
      for(j = 0; j < 11; j++){
        if(finfo[i].name[j] != s[j]){
          found=0;
          break;
        }
      }
      if(found==1){
        return finfo + i;
      }
    }
    i++;
  }
  return 0; 
}
void file_readfat(int *fat,unsigned char *img){
  int j=0;
  for(int i=0;i<2880;i+=2){
    fat[i+0] = (img[j + 0])      | (img[j + 1] << 8 & 0xfff);
    fat[i+1] = (img[j + 1]) >> 4 | (img[j + 2] << 4 & 0xfff);
    j+=3;
  }
  return;
}
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img){
  while(1){
    if(size<=512){
      for(int i=0;i<size;i++){
        buf[i] = img[clustno*512+i];
      }
      break;
    }
    for(int i=0;i<512;i++){
      buf[i] = img[clustno*512+i];
    }
    size -= 512;
    buf += 512;
    clustno = fat[clustno];
  }
  return;
}
