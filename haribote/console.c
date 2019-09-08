#include"bootpack.h"
void cons_putstr0(CONSOLE *cons,const char *s){
  for(;*s!=0;s++){
    cons_putchar(cons,*s,1);
  }
  return;
}
void cons_putstr1(CONSOLE *cons,const char *s,int size){
  for(int i=0;i<size;i++){
    if(s[i]==0){
      break;
    }
    cons_putchar(cons,s[i],1);
  }
  return;
}
void hrb_api_linewin(SHEET *sht,int x0,int y0,int x1,int y1,int col){
  int x,y,dx,dy,len;
  dx = x1-x0;
  dy = y1-y0;
  x = x0<<10;
  y = y0<<10;
  dx *= (dx<0)?-1:1;
  dy *= (dy<0)?-1:1;
  if(dx>=dy){
    len = dx+1;
    dx = (x0>x1)?-1024:1024;
    if(y0<=y1){
      dy = ((y1-y0+1)<<10)/len;
    }else{
      dy = ((y1-y0-1)<<10)/len;
    }
  }else{
    len = dy+1;
    dy = (y0>y1)?-1024:1024;
    if(x0<=x1){
      dx = ((x1-x0+1)<<10)/len;
    }else{
      dx = ((x1-x0-1)<<10)/len;
    }
  }
  for(int i=0;i<len;i++){
    sht->buf[(y>>10)*sht->bxsize + (x>>10)]=col;
    x += dx;
    y += dy;
  }
}
int *inthandler0c(int *esp){
  TASK *task = task_now();
  CONSOLE *cons = task->cons;
  char s[30];
  cons_putstr0(cons,"\nINT 0c :\n Stack Exception\n");
  my_sprintf(s,"EIP = %x\n",esp[11]);
  cons_putstr0(cons,s);
  return &(task->tss.esp0);
}
int *inthandler0d(int *esp){
  TASK *task = task_now();
  CONSOLE *cons = task->cons;
  char s[30];
  cons_putstr0(cons,"\nINT 0d\n Protected Exception\n");
  my_sprintf(s,"EIP = %x\n",esp[11]);
  cons_putstr0(cons,s);
  return &(task->tss.esp0);
}
void cons_newline(CONSOLE *cons){
  SHEET *sheet = cons->sht;
  TASK *task = task_now();
  if(cons->cur_y<28+112){
    cons->cur_y+=16;
  }else{
    if(sheet!=0){
      for(int y=28;y<28+112;y++){
        for(int x=8;x<8+240;x++){
          sheet->buf[x+y*sheet->bxsize] = 
           sheet->buf[x+(y+16)*sheet->bxsize];
        }
      }
      for(int y=28+112;y<28+128;y++){
        for(int x=8;x<8+240;x++){
          sheet->buf[x+y*sheet->bxsize]=COL8_000000;
        }
      }
      sheet_refresh(sheet,8,28,8+240,28+128);
    }
  }
  cons->cur_x=8;
  if(task->langmode==1&&task->langbyte1!=0){
    cons->cur_x+=8;
  }
  return;
}
int cmd_app(CONSOLE *cons,int *fat,char *cmdline){
  MEMMAN *memman = (MEMMAN*)MEMMAN_ADDR;
  FILEINFO *finfo;
  SHTCTL *shtctl;
  SHEET *sht;
  TASK *task = task_now();
  char name[30],*p,*q;
  int i,segsize,datsize,esp,dathrb,appsize;
  for(i=0;i<24;i++){
    if(cmdline[i]<=' '){
      break;
    }
    name[i] = cmdline[i];
  }
  name[i]=0;
  finfo = file_search(name,(FILEINFO*)(ADR_DISKIMG+0x000200),272);
  if(finfo==0&&name[i-1]!='.'){
    name[i+0] = '.';
    name[i+1] = 'H';
    name[i+2] = 'R';
    name[i+3] = 'B';
    name[i+4] =  0 ;
    finfo = file_search(name,(FILEINFO*)(ADR_DISKIMG+0x000200),272);
    if(finfo==0){
      name[i+1] = 'h';
      name[i+2] = 'r';
      name[i+3] = 'b';
      finfo = file_search(name,(FILEINFO*)(ADR_DISKIMG+0x000200),272);
    }
  }
  if(finfo!=0){
    appsize =finfo->size;
    p = (char*)memman_alloc_4k(memman,finfo->size);
    file_loadfile2(finfo->clustno,&appsize);
    cons_putstr0(cons,p);
    cons_putstr0(cons,"nakami");
    if(finfo->size>=36&&strn_cmp(p+4,"Hari",4)==0&&*p==0x00){
      segsize = *((int*)(p+0x0000));
      esp     = *((int*)(p+0x000c));
      datsize = *((int*)(p+0x0010));
      dathrb  = *((int*)(p+0x0014));
      q = (char*)memman_alloc_4k(memman,segsize); //使用可能メモリ
      task->ds_base = (int)q;
      set_segmdesc(task->ldt+0,finfo->size-1,(int)p,AR_CODE32_ER+0x60);
      set_segmdesc(task->ldt+1,segsize - 1  ,(int)q,AR_DATA32_RW+0x60);
      for(int i=0;i<datsize;i++){
        q[esp+i] = p[dathrb+i];
      }
      start_app(0x1b, 0*8+4 ,esp,1*8+4,&(task->tss.esp0));
      shtctl = (SHTCTL*)*((int*)0x0fe4);
      for(int i=0;i<MAX_SHEETS;i++){
        sht = &(shtctl->sheets0[i]);
        if((sht->flags&0x11)==0x11&&sht->task==task){
          sheet_free(sht);
        }
      }
      for(int i=0;i<8;i++){
        if(task->fhandle[i].buf!=0){
          memman_free_4k(memman,(int)task->fhandle[i].buf,task->fhandle[i].size);
          task->fhandle[i].buf=0;
        }
      }
      timer_cancelall(&task->fifo);
      memman_free_4k(memman,(int)q,segsize);
      task->langbyte1=0;
    }else{
      cons_putstr0(cons,".hrb file format error\n");
    }
    memman_free_4k(memman,(int)p,finfo->size);
    cons_newline(cons);
    return 1;
  }
  return 0;
}
void cmd_ncst(CONSOLE* cons,char *cmdline,int memtotal){
  TASK *task = open_constask(0,memtotal);
  FIFO32 *fifo = &task->fifo;
  for(int i=5;cmdline[i]!=0;i++){
    fifo32_put(fifo,cmdline[i]+256);
  }
  fifo32_put(fifo,10+256);
  cons_newline(cons);
  return;
}
void cmd_start(CONSOLE *cons,char *cmdline,int memtotal){
  SHTCTL *shtctl = (SHTCTL*)*((int*)0x0fe4);
  SHEET *sht = open_console(shtctl,memtotal);
  FIFO32 *fifo = &sht->task->fifo;
  sheet_slide(sht,32,4);
  sheet_updown(sht,shtctl->top);
  for(int i=6;cmdline[i]!=0;i++){
    fifo32_put(fifo,cmdline[i]+256);
  }
  fifo32_put(fifo,10+256);
  cons_newline(cons);
  return;
}
void cmd_exit(CONSOLE *cons,int *fat){
  MEMMAN *memman = (MEMMAN*)MEMMAN_ADDR;
  TASK *task = task_now();
  SHTCTL *shtctl = (SHTCTL*)*((int*)0x0fe4);
  FIFO32 *fifo = (FIFO32*)*((int*)0x0fec);
  if(cons->sht!=0){
    timer_cancel(cons->timer);
  }
  memman_free_4k(memman,(int)fat,4*2880);
  io_cli();
  if(cons->sht!=0){
    fifo32_put(fifo,cons->sht - shtctl->sheets0+768);
  }else{
    fifo32_put(fifo,task-taskctl->tasks0+1024);
  }
  io_sti();
  while(1){
    task_sleep(task);
  }
}
void cons_putchar(CONSOLE *cons,int chr,int move){
  char s[2];
  s[0]=chr;
  s[1]=0;
  //tab
  if(s[0]==0x09){
    while(1){
      if(cons->sht!=0){
        putfont8_str_sht(cons->sht,cons->cur_x,cons->cur_y,COL8_FFFFFF,COL8_000000," ",1);
      }
      cons->cur_x += 8;
      if(cons->cur_x == 8+240){
        cons->cur_x = 8;
        cons_newline(cons);
      }
      //32で割り切れる
      if(((cons->cur_x-8)&0x1f)==0){
        break;
      }
    }
    //改行
  }else if(s[0]==0x0a){
    cons_newline(cons);
    //リカバリ
  }else if(s[0]==0x0d){
    //なにもしません
  }else{
    if(cons->sht!=0){
      putfont8_str_sht(cons->sht,cons->cur_x,cons->cur_y,COL8_FFFFFF,COL8_000000,s,1);
    }
    if(move!=0){
      cons->cur_x+=8;
      if(cons->cur_x==8+240){
        cons_newline(cons);
      }
    }
  }
  return;
}
void cmd_free(CONSOLE *cons,unsigned int memtotal){
  char s[30];
  MEMMAN *memman = (MEMMAN*) MEMMAN_ADDR;
  my_sprintf(s,"total %dMB free %dKB\n\n",memtotal/(1024*1024),memman_total(memman)/1024);
  cons_putstr0(cons,s);
}
void cmd_langmode(CONSOLE *cons,char *cmdline){
  TASK *task=task_now();
  unsigned char mode = cmdline[9]-'0';
  if(mode<=2){
    task->langmode=mode;
  }else{
    cons_putstr0(cons,"mode number error\n");
  }
  cons_newline(cons);
  return;
}
void cmd_clean(CONSOLE *cons){
  for(int y=0;y<28+128;y++){
    for(int x=0;x<8+240;x++){
      cons->sht->buf[x+y*cons->sht->bxsize]=COL8_000000;
    }
  }
  sheet_refresh(cons->sht,8,28,8+240,28+128);
  cons->cur_y = 28;
  return;
}

void cmd_ls(CONSOLE *cons){
  FILEINFO *finfo = (FILEINFO*)(ADR_DISKIMG+0x000200);
  char s[31];
  for(int i=0;i<272;i++){
    if(finfo[i].size==-1){
      break;
    }
    my_sprintf(s,"                        %d\n",finfo[i].size);
    for(int j=0;j<24;j++){
      if(finfo[i].name[j]==0){
        break;
      }
      s[j]=finfo[i].name[j];
    }
    cons_putstr0(cons,s);
  }
  cons_newline(cons);
  return;
}
void cmd_cat(CONSOLE *cons,int *fat,char *cmdline){
  FILEINFO *finfo = finfo = file_search(cmdline+4,(FILEINFO*)(ADR_DISKIMG+0x000200),272);

  MEMMAN *memman = (MEMMAN*) MEMMAN_ADDR;
  char *p;
  if(finfo!=0){
    p = (char*)memman_alloc_4k(memman,finfo->size);
    file_loadfile2(finfo->clustno,&finfo->size);
    cons_putstr1(cons,p,finfo->size);
    memman_free_4k(memman,(int)p,finfo->size);
  }else{
    cons_putstr0(cons,"File not Found :-P\n");
  }
  cons_newline(cons);
}
void cmd_hlt(CONSOLE *cons,int *fat){
  FILEINFO *finfo = file_search("HLT.HRB",(FILEINFO*)(ADR_DISKIMG+0x002600),224);
  MEMMAN *memman = (MEMMAN*) MEMMAN_ADDR;
  SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR*)ADR_GDT;
  char *p;
  if(finfo!=0){
    p = (char*)memman_alloc_4k(memman,finfo->size);
    file_loadfile2(finfo->clustno,&finfo->size);
    set_segmdesc(gdt+1003,finfo->size-1,(int)p,AR_CODE32_ER);
    farcall(0,1003*8);
    memman_free_4k(memman,(int)p,finfo->size);
  }else{
    cons_putstr0(cons,"Oops! file not found\n");
  }
  cons_newline(cons);
}
void cons_runcmd(CONSOLE *cons,char *cmdline,int *fat,unsigned int memtotal){
  if(strcmp(cmdline,"free")==0&&cons->sht!=0){
    cmd_free(cons,memtotal);
  }else if(strcmp(cmdline,"clean")==0&&cons->sht!=0){
    cmd_clean(cons);
  }else if(strcmp(cmdline,"ls")==0&&cons->sht!=0){
    cmd_ls(cons);
  }else if(strcmp(cmdline,"sugee")==0&&cons->sht!=0){
    cons_putstr0(cons,"sugoine\n");
  }else if(strcmp(cmdline,"exit")==0){
    cmd_exit(cons,fat);
  }else if(strn_cmp(cmdline,"start ",6)==0){
    cmd_start(cons,cmdline,memtotal);
  }else if(strn_cmp(cmdline,"ncst ",5)==0){
    cmd_ncst(cons,cmdline,memtotal);
  }else if(strn_cmp(cmdline,"langmode ",9)==0){
    cmd_langmode(cons,cmdline);
  }else if(strn_cmp(cmdline,"cat ",4)==0){
    cmd_cat(cons,fat,cmdline);
  }else if(cmdline[0]!=0){
    if(cmd_app(cons,fat,cmdline)==0){
      cons_putstr0(cons,"No such commands :-D");
      cons_newline(cons);
      cons_newline(cons);
    }
  }
  return;
}
int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax){
  char tmp[12];
  TASK *task = task_now();
  int ds_base = task->ds_base,info;
  CONSOLE *cons = task->cons;
  SHTCTL *shtctl = (SHTCTL *)*((int*)0x0fe4);
  FIFO32 *sys_fifo = (FIFO32*)*((int*)0x0fec);
  FILEINFO *finfo;
  FILEHANDLE *fh;
  MEMMAN *memman = (MEMMAN*)MEMMAN_ADDR;
  SHEET *sht;
  int *reg = &eax+1;
  switch(edx){
    case 1:
      cons_putchar(cons, eax&0xff,1);
      break;
    case 2:
      cons_putstr0(cons,(char*)ebx+ds_base);
      break;
    case 3:
      cons_putstr1(cons,(char*)ebx+ds_base,ecx);
      break;
    case 4:
      return &(task->tss.esp0);
      break;
    case 5:
      sht = sheet_alloc(shtctl);
      sht->task = task;
      sht->flags |= 0x10;
      sheet_setbuf(sht,(char*)ebx+ds_base,esi,edi,eax);
      make_window8((char*)ebx+ds_base,esi,edi,(char*)ecx+ds_base,0);
      sheet_slide(sht,(shtctl->xsize-esi)/2 & ~3,(shtctl->ysize-esi)/2);
      sheet_updown(sht,shtctl->top);
      reg[7] = (int)sht;
      break;
    case 6:
      sht = (SHEET*) (ebx&0xfffffffe);
      putfont8_str(sht->buf,sht->bxsize,esi,edi,eax,(char*)ebp+ds_base);
      if((ebx&1)==0){
        sheet_refresh(sht,esi,edi,esi+ecx*8,edi+16);
      }
      break;
    case 7:
      sht = (SHEET*) (ebx&0xfffffffe);
      boxfill8(sht->buf,sht->bxsize,ebp,eax,ecx,esi,edi);
      if((ebx&1)==0){
        sheet_refresh(sht,eax,ecx,esi+1,edi+1);
      }
      break;
    case 8:
      memman_init((MEMMAN*)(ebx+ds_base));
      ecx &= 0xfffffff0;
      memman_free((MEMMAN*)(ebx+ds_base),eax,ecx);
      break;
    case 9:
      ecx = (ecx+0x0f)&0xfffffff0;
      reg[7] = memman_alloc((MEMMAN*)(ebx+ds_base),ecx);
      break;
    case 10:
      ecx = (ecx+0x0f)&0xfffffff0;
      memman_free((MEMMAN*)(ebx+ds_base),eax,ecx);
      break;
    case 11:
      sht = (SHEET*) (ebx&0xfffffffe);
      sht->buf[sht->bxsize*edi+esi]=eax;
      if((ebx&1)==0){
        sheet_refresh(sht,esi,edi,esi+1,edi+1);
      }
      break;
    case 12:
      sht = (SHEET *)ebx;
      sheet_refresh(sht,eax,ecx,esi,edi);
      break;
    case 13:
      sht = (SHEET*)(ebx&0xfffffffe);
      hrb_api_linewin(sht,eax,ecx,esi,edi,ebp);
      if((ebx&1)==0){
        if(eax>esi){
          info=eax;
          eax=esi;
          esi=info;
        }
        if(ecx>edi){
          info=ecx;
          ecx=edx;
          edi=info;
        }
        sheet_refresh(sht,eax,ecx,esi+1,edi+1);
      }
      break;
    case 14:
      sheet_free((SHEET*)ebx);
      break;
    case 15:
      while(1){
        io_cli();
        if(fifo32_statue(&task->fifo)==0){
          if(eax!=0){
            task_sleep(task);
          }else{
            io_sti();
            reg[7]=-1;
            return 0;
          }
        }
        info = fifo32_get(&task->fifo);
        io_sti();
        if(info<=1){
          timer_init(cons->timer,&task->fifo,1);
          timer_settime(cons->timer,50);
        }
        if(info==2){
          cons->cur_c = COL8_FFFFFF;
        }
        if(info==3){
          cons->cur_c = -1;
        }
        if(info==4){
          timer_cancel(cons->timer);
          io_cli();
          fifo32_put(sys_fifo,cons->sht-shtctl->sheets0+2024);
          cons->sht=0;
          io_sti();
        }
        if(256<=info){
          reg[7]=info-256;
          return 0;
        }
      }
      break;
    case 16:
      reg[7] = (int)timer_alloc();
      ((TIMER*)reg[7])->flags2 = 1;
      break;
    case 17:
      timer_init((TIMER*)ebx,&task->fifo,eax+256);
      break;
    case 18:
      timer_settime((TIMER*)ebx,eax);
      break;
    case 19:
      timer_free((TIMER*)ebx);
      break;
    case 20:
     if(eax==0) eax=1;
     info = 1193180000/eax;
     io_out8(0x43,0xb6);
     io_out8(0x42,info&0xff);
     io_out8(0x42,info>>8);
     info = io_in8(0x61);
     io_out8(0x61,(info|0x03)&0x0f);
     break;
    case 21:
     for(info=0;info<8;info++){
       if(task->fhandle[info].buf==0){
         break;
       }
     }
     fh = &task->fhandle[info];
     reg[7] = 0;
     if(info<8){
       finfo = file_search((char*)ebx+ds_base,
           (FILEINFO*)(ADR_DISKIMG+0x002600),224);
       if(finfo!=0){
         reg[7]=(int)fh;
         fh->buf = (char*)memman_alloc_4k(memman,finfo->size);
         fh->size = finfo->size;
         fh->pos = 0;
         file_loadfile2(finfo->clustno,&finfo->size);
       }
     }
     break;
    case 22:
     fh = (FILEHANDLE *)eax;
     memman_free_4k(memman,(int)fh->buf,fh->size);
     fh->buf=0;
     break;
    case 23:
     fh = (FILEHANDLE*) eax;
     if(ecx==0){
       fh->pos=ebx;
     }else if(ecx==1){
       fh->pos+=ebx;
     }else if(ecx==2){
       fh->pos = fh->size + ebx;
     }
     if(fh->pos<0){
       fh->pos=0;
     }
     if(fh->pos>fh->size){
       fh->pos = fh->size;
     }
     break;
    case 24:
     fh = (FILEHANDLE*)eax;
     if(ecx==0){
       reg[7]=fh->size;
     }else if(ecx==1){
       reg[7]=fh->pos;
     }else if(ecx==2){
       reg[7]=fh->pos-fh->size;
     }
     break;
    case 25:
     fh = (FILEHANDLE*)eax;
     for(info=0;info<ecx;info++){
       if(fh->pos==fh->size){
         break;
       }
       *((char*)ebx+ds_base+info) = fh->buf[fh->pos];
       fh->pos++;
     }
     reg[7]=info;
     break;
    case 26:
     info=0;
     while(1){
       *((char*)ebx+ds_base+info) = task->cmdline[info];
       if(task->cmdline[info]==0){
         break;
       }
       if(info>=ecx){
         break;
       }
       info++;
     }
     reg[7]=info;
     break;
    case 27:
     reg[7] = task->langmode;
     break;
  }
  return 0;
}
void console_task(SHEET *sheet,unsigned int memtotal){
  TASK *task = task_now();
  MEMMAN *memman = (MEMMAN*)MEMMAN_ADDR;
  FILEHANDLE fhandle[8];
  unsigned char *nihongo = (char*)*((int*)0x0fe8);
  int info,*fat = (int *)memman_alloc_4k(memman,4*2880);
  char cmdline[30];
  CONSOLE cons;
  cons.sht=sheet;
  cons.cur_x=8;
  cons.cur_y=28;
  cons.cur_c=-1;
  task->cons = &cons;
  task->cmdline = cmdline;
  if(cons.sht!=0){
    cons.timer = timer_alloc();
    timer_init(cons.timer, &task->fifo,0);
    timer_settime(cons.timer,50);
  }

  for(int i=0;i<8;i++){
    fhandle[i].buf=0;
  }
  if(nihongo[4096]!=0xff){
    task->langmode=1;
  }else{
    task->langmode=0;
  }
  task->langbyte1=0;
  task->fhandle = fhandle;
  task->fat = fat;

  cons_putchar(&cons,'>',1);

  while(1){
    io_cli();
    if(fifo32_statue(&task->fifo)==0){
      task_sleep(task);
      io_sti();
    }else{
      info = fifo32_get(&task->fifo);
      io_sti();
      if(info<=1&&cons.sht!=0){
        if(info==0){
          timer_init(cons.timer,&task->fifo,1);
          if(cons.cur_c>=0){
            cons.cur_c = COL8_000000;
          }
        }else if(info==1){
          timer_init(cons.timer,&task->fifo,0);
          if(cons.cur_c>=0){
            cons.cur_c = COL8_FFFFFF;
          }
        }
        timer_settime(cons.timer,50);
      }
      if(info==2){
        cons.cur_c = COL8_FFFFFF;
      }
      if(info==3){
        if(cons.sht!=0){
          boxfill8(sheet->buf,cons.sht->bxsize,
              COL8_000000,cons.cur_x,cons.cur_y,cons.cur_x+7,cons.cur_y+15);
        }
        cons.cur_c=-1;
      }
      if(info==4){
        cmd_exit(&cons,fat);
      }
      if(256<=info&&info<=511){
        if(info == 8+256){
          if(cons.cur_x>16){
            cons_putchar(&cons,' ',0);
            cons.cur_x-=8;
          }
        }else if(info == 10+256){ //enter
          cons_putchar(&cons,' ',0);
          cmdline[cons.cur_x/8 -2]=0;
          cons_newline(&cons);
          cons_runcmd(&cons,cmdline,fat,memtotal);
          if(cons.sht==0){
            cmd_exit(&cons,fat);
          }
          cons_putchar(&cons,'>',1);
        }else{
          if(cons.cur_x<240){
            cmdline[cons.cur_x/8 -2]=info-256;
            cons_putchar(&cons,info-256,1);
          }
        }
      }
      if(cons.sht!=0){
        if(cons.cur_c>=0){
          boxfill8(cons.sht->buf,sheet->bxsize,
              cons.cur_c,cons.cur_x,cons.cur_y,cons.cur_x+7,cons.cur_y+15);
        }
        sheet_refresh(cons.sht,cons.cur_x,cons.cur_y,cons.cur_x+8,cons.cur_y+16);
      }
    }
  }
}
