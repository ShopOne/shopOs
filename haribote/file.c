#include"bootpack.h"
void file_loadfile(int clustno, int size, char *buf, char *img){
  int i;
  for (i = 0; i < size; i++){
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
void file_init(){
  FILEINFO *finfo = (FILEINFO*)(ADR_DISKIMG+0x000200);
  char s[31];
  for(int i=0;i<272;i++){
    if(finfo[i].size==-1){
      break;
    }
    my_sprintf(s, "                      %d\n", finfo[i].size);
    for(int j=0;j<24;j++){
      if(finfo[i].name[j]==0){
        break;
      }
      s[j]=finfo[i].name[j];
    }
  }
  return;
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
DIRMAN* newdir_init(DIRMAN *parent_dir,DIRMAN *next_dir,char *name){
  MEMMAN *memman = (MEMMAN *) MEMMAN_ADDR;
  DIRMAN *new_dir = (DIRMAN*)memman_alloc_4k(memman,sizeof(DIRMAN));
  new_dir->parentdir=parent_dir;
  new_dir->curdir = new_dir;
  new_dir->next_dnum = 0;
  new_dir->next_fnum = 0;
  for(int i=0;name[i]!=0;i++){
    new_dir->name[i]=name[i];
  }
  return new_dir;
}
DIRMAN* make_dir(DIRMAN *now_dir,char* dname){
  if(now_dir->next_dnum<MAX_DIR){
    return now_dir->nextdirs[now_dir->next_dnum++] = newdir_init(now_dir,0,dname);
  }else{
    return 0;
  }
}
DIRMAN *search_dir(DIRMAN *now_dir,char *name){
  for(int i=0;i<now_dir->next_dnum;i++){
    if(strcmp(now_dir->nextfiles[i]->name,name)==0){
      return now_dir;
    }
  }
  return 0;
}
void init_files(){
  FILEINFO *finfo = (FILEINFO*)(ADR_DISKIMG+0x000200);
  char s[31];
  // 親ディレクトリ
  MEMMAN *memman = (MEMMAN *) MEMMAN_ADDR;
  DIRMAN *home_dir = (DIRMAN*)*((int*)HOMEDIR_ADDR),*now_dir;
  home_dir = (DIRMAN*)memman_alloc_4k(memman,sizeof(home_dir));
  home_dir->name[0]='/';
  home_dir->name[1]=0;
  home_dir->next_fnum=0;
  home_dir->next_dnum=0;
  DIRMAN *tmp_dir;
  int sj;
  for(int i=0;i<272;i++){
    if(finfo[i].size==-1){
      break;
    }
    now_dir = home_dir;
    for(int j=0,sj=0;j<31;j++,sj++){
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
        s[0]=0;
        sj=-1;
      }
    }
    if(now_dir->next_fnum<MAX_FILE){
      now_dir->nextfiles[now_dir->next_fnum++] = finfo;
    }
  }
  return;
}
