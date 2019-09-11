#include"bootpack.h"
void file_loadfile(int clustno, int size, char *buf, char *img){
  int i;
  for (i = 0; i < size; i++) {
    buf[i] = img[clustno + i];
  }
  return;
}

void file_writefile(int clustno, int size, char *buf,char *img){
  for(int i=0;i<size;i++){
    img[clustno+i] = buf[i];
  }
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

void file_writefile2(int clustno, int *psize,char *buf){
  int size = *psize;
  file_writefile(clustno,size,buf,(char*)ADR_DISKIMG);
}
char *file_loadfile2(int clustno, int *psize){
  int size = *psize;
  MEMMAN *memman = (MEMMAN *) MEMMAN_ADDR;
  char *buf;
  buf = (char *) memman_alloc_4k(memman, size);
  file_loadfile(clustno, size, buf, (char *) ADR_DISKIMG);
  return buf;
}
DIRINFO* newdir_init(DIRINFO *parent_dir,DIRINFO *next_dir,char *name){
  MEMMAN *memman = (MEMMAN *) MEMMAN_ADDR;
  DIRINFO *new_dir = (DIRINFO*)memman_alloc_4k(memman,sizeof(DIRINFO));
  new_dir->parentdir=parent_dir;
  new_dir->curdir = new_dir;
  new_dir->nextdir = next_dir;
  for(int i=0;name[i]!=0;i++){
    new_dir->name[i]=name[i];
  }
  return new_dir;
}
DIRINFO* make_dir(DIRINFO *now_dir,char* dname){
  DIRINFO *p,*q;
  p = now_dir->nextdir;
  q = 0;
  // ディレクトリ内にディレクトリが自分しかいない
  if(p==0){
    q=now_dir;
  }else{
    while(p!=0){
      q=p;
      p=p->nextdir;
    }
  }
  q->nextdir=newdir_init(now_dir,0,dname);
  return q->nextdir;
}
DIRINFO *search_dir(DIRINFO *now_dir,char *name){
  while(now_dir!=0){
    if(strcmp(now_dir->name,name)==0){
      return now_dir;
    }
  }
  return 0;
}
FILEINFO *write_fileinfo(DIRINFO *now_dir,char *name){
  MEMMAN *memman = (MEMMAN *) MEMMAN_ADDR;
  FILEINFO *newfile = (FILEINFO*)memman_alloc_4k(memman,sizeof(newfile));
}
FILEINFO *make_file(DIRINFO *now_dir,char *name){
}
void init_files(){
  FILEINFO *finfo = (FILEINFO*)(ADR_DISKIMG+0x000200);
  char s[31];
  // 親ディレクトリ
  DIRINFO *now_dir = (DIRINFO*)*((int*)HOMEDIR_ADDR);
  DIRINFO *tmp_dir;
  for(int i=0;i<272;i++){
    if(finfo[i].size==-1){
      break;
    }
    for(int j=0;j<31;j++){
      if(finfo[i].name[j]==0){
        break;
      }
      if(s[j]!='/'){
        s[j]=finfo[i].name[j];
      }else{
        tmp_dir = search_dir(now_dir,s);
        if(tmp_dir==0){
          now_dir = make_dir(now_dir,s);
        }else{
          now_dir = tmp_dir;
        }
      }
    }
  }
  return;
}
