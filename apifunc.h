#include<stdarg.h> 
#include <limits.h>
#include <ctype.h>
#include <errno.h>
void api_putchar(int c);
void api_putstr0(char *s);
void api_point(int win,int x,int y,int col);
int  api_openwin(char* buf,int xsize,int ysize,int col_inv,char *title);
void api_putstrwin(int win,int x,int y,int col,int len,char *str);
void api_boxfillwin(int win,int x0,int y0,int x1,int y1,int col);
void api_initmalloc();
char *api_malloc(int size);
void api_linewin(int win,int x0,int y0,int x1,int y1,int col); 
void api_closewin(int win);
void api_end();
int api_alloctimer();
void api_settimer(int timer,int time);
void api_inittimer(int timer,int data);
int api_getkey(int mode);
void srand(unsigned int seed);
int rand();
void api_refreshwin(int win,int x0,int y0,int x1,int y1);
void my_sprintf (char *str, char *fmt, ...);
void api_beep(int tone);
int api_fopen(char *fname);
void api_fclose(int fhandle);
void api_fseek(int fhandle,int offset,int mode);
int api_fsize(int fhandle,int mode);
int api_fread(char *buf,int maxsize,int fhandle);
int api_cmdline(char *buf,int maxsize);
int api_getlang();
#define RAND_MAX 500



//functional
int rand_val;
void srand(unsigned int seed) {
    rand_val = seed;
}

int rand() {
    rand_val = (1231053 * rand_val + 13245) % RAND_MAX;
    rand_val *= (rand_val<0)?-1:1;
    return rand_val;
}
//10進数からASCIIコードに変換
int dec2asc (char *str, int dec) {
    int len = 0, len_buf; //桁数
    int buf[10];
    while (1) { //10で割れた回数（つまり桁数）をlenに、各桁をbufに格納
        buf[len++] = dec % 10;
        if (dec < 10) break;
        dec /= 10;
    }
    len_buf = len;
    while (len) {
        *(str++) = buf[--len] + 0x30;
    }
    return len_buf;
}
 
//16進数からASCIIコードに変換
int hex2asc (char *str, int dec) { //10で割れた回数（つまり桁数）をlenに、各桁をbufに格納
    int len = 0, len_buf; //桁数
    int buf[10];
    while (1) {
        buf[len++] = dec % 16;
        if (dec < 16) break;
        dec /= 16;
    }
    len_buf = len;
    while (len) {
        len --;
        *(str++) = (buf[len]<10)?(buf[len] + 0x30):(buf[len] - 9 + 0x60);
    }
    return len_buf;
}
 
void my_sprintf (char *str, char *fmt, ...) {
    va_list list;
    int  len;
    va_start (list, fmt);
 
    while (*fmt) {
        if(*fmt=='%') {
            fmt++;
            switch(*fmt){
                case 'd':
                    len = dec2asc(str, va_arg (list, int));
                    break;
                case 'x':
                    len = hex2asc(str, va_arg (list, int));
                    break;
            }
            str += len; fmt++;
        } else {
            *(str++) = *(fmt++);
        }   
    }
    *str = 0x00; //最後にNULLを追加
    va_end (list);
}
int putchar(int c){
  api_putchar(c);
  return c;
}
int strcmp(const char* s,const char *t){
  int i=0,j=0;
  while(s[i]!=0&&t[j]!=0){
    if(s[i]!=t[j]){
      return 1;
    }
    i++;
    j++;
  }
  if(s[i]!=t[j]) return 1;
  else return 0;
}
int str_len(const unsigned char *s){
  int i=0;
  while(s[i]!=0){
    i++;
  }
  return i;
}
