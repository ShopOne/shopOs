#include"bootpack.h"
void file_loadfile(int clustno, int size, char *buf, char *img)
{
  int i;
  for (i = 0; i < size; i++) {
    buf[i] = img[clustno + i];
  }
  return;
}

 FILEINFO *file_search(char *name,  FILEINFO *finfo, int max){
  int i;
  for (i = 0; i < max; i++) {
    if (finfo->size == -1) {
      break;
    }
    if (strn_cmp(name, (char*)finfo[i].name, 24) == 0) {
      return finfo + i;
    }
  }
  return 0;
}

char *file_loadfile2(int clustno, int *psize){
  int size = *psize, size2;
  MEMMAN *memman = ( MEMMAN *) MEMMAN_ADDR;
  char *buf, *buf2;
  buf = (char *) memman_alloc_4k(memman, size);
  file_loadfile(clustno, size, buf, (char *) ADR_DISKIMG);
  return buf;
}
