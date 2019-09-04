#include"bootpack.h"

SHTCTL *shtctl_init(MEMMAN *memman,unsigned char *vram,int xsize,int ysize){
  SHTCTL *ctl;
  ctl = (SHTCTL*) memman_alloc_4k(memman,sizeof(SHTCTL));
  if(ctl!=0){
    ctl->map = (unsigned char *)memman_alloc_4k(memman,xsize*ysize);
    if(ctl->map==0){
      memman_free_4k(memman,(int)ctl,sizeof(SHTCTL));
    }else{
      ctl->vram = vram;
      ctl->xsize=xsize;
      ctl->ysize=ysize;
      ctl->top=-1;
      for(int i=0;i<MAX_SHEETS;i++){
        ctl->sheets0[i].flags=0;
        ctl->sheets0[i].ctl=ctl;
      }
    }
  }
  return ctl;
}
SHEET *sheet_alloc(SHTCTL *ctl){
  SHEET *sht;
  for(int i=0;i<MAX_SHEETS;i++){
    if(ctl->sheets0[i].flags==0){
      sht = &ctl->sheets0[i];
      sht->flags = SHEET_USE;
      sht->height = -1;
      sht->task = 0;
      return sht;
    }
  }
  return 0;
}
void sheet_refresh_map(SHTCTL *ctl,int vx0,int vy0, int vx1,int vy1,int h0){
  int vx,vy,bx0,by0,bx1,by1,sid4,*p;
  unsigned char *buf,sid,*map=ctl->map;
  SHEET *sht;
  vx0 = max(0,vx0);
  vy0 = max(0,vy0);
  vx1 = min(vx1,ctl->xsize);
  vy1 = min(vy1,ctl->ysize);
  for(int h=h0;h<=ctl->top;h++){
    sht = ctl->sheets[h];
    sid = sht - ctl->sheets0;
    buf = sht->buf;
    bx0 = max(0,vx0 - sht->vx0);
    by0 = max(0,vy0 - sht->vy0);
    bx1 = min(sht->bxsize, vx1-sht->vx0);
    by1 = min(sht->bysize, vy1-sht->vy0);
    if(sht->col_inv==-1){
      if((sht->vx0&3)==0&&(bx0&3)==0&&(bx1&3)==0){
        bx1 = (bx1-bx0)/4;
        sid4 = (sid)|(sid<<8)|(sid<<16)|(sid<<24);
        for(int by=by0;by<by1;by++){
          vy = sht->vy0+by;
          vx = sht->vx0+bx0;
          p = (int*)&map[vy*ctl->xsize+vx];
          for(int bx=0;bx<bx1;bx++){
            p[bx]=sid4;
          }
        }
      }else{
        for(int by=by0;by<by1;by++){
          vy = sht->vy0+by;
          for(int bx=bx0;bx<bx1;bx++){
            vx = sht->vx0+bx;
            map[vy*ctl->xsize+vx]=sid;
            if(vy*ctl->xsize+vx<0){
              map[vy*ctl->xsize+vx]=3;
            }
          }
        }
      }
    }else{
      for(int by=by0;by<by1;by++){
        vy = sht->vy0+by;
        for(int bx=bx0;bx<bx1;bx++){
          vx = sht->vx0+bx;
          if(buf[by*sht->bxsize+bx]!=sht->col_inv){
            map[vy*ctl->xsize+vx]=sid;
            if(vy*ctl->xsize+vx<0){
              map[vy*ctl->xsize+vx]=3;
            }
          }
        }
      }
    }
  }
  return;
}
void sheet_setbuf(SHEET *sht, unsigned char *buf,int xsize, int ysize, int col_inv){
  sht->buf=buf;
  sht->bxsize=xsize;
  sht->bysize=ysize;
  sht->col_inv=col_inv;
  return;
}
void sheet_refresh_sub(SHTCTL *ctl,int vx0,int vy0,int vx1,int vy1,int h0,int h1){
  int bx,by,h,vx,vy,bx0,bx1,by0,by1,bx2,sid4,i,i1,*p,*q,*r;
  unsigned char *buf,*vram=ctl->vram,sid,*map=ctl->map;
  SHEET *sht;
  vx0 = max(0,vx0);
  vy0 = max(0,vy0);
  vx1 = min(ctl->xsize,vx1);
  vy1 = min(ctl->ysize,vy1);
  for(int h=h0;h<=h1;h++){
    sht = ctl->sheets[h];
    buf = sht->buf;
    sid = sht - ctl->sheets0;
    bx0 = vx0 - sht->vx0;
    by0 = vy0 - sht->vy0;
    bx1 = vx1 - sht->vx0;
    by1 = vy1 - sht->vy0;
    if(bx0 < 0){ bx0 = 0; }
    if(by0 < 0){ by0 = 0; }
    if(bx1 > sht->bxsize){ bx1 = sht->bxsize; }
    if(by1 > sht->bysize){ by1 = sht->bysize; }
    if((sht->vx0 & 3) == 0){
      i  = (bx0 + 3) / 4; 
      i1 =  bx1      / 4; 
      i1 = i1 - i;
      sid4 = sid | sid << 8 | sid << 16 | sid << 24;
      for(by = by0; by < by1; by++){
        vy = sht->vy0 + by;
        for(bx = bx0; bx < bx1 && (bx & 3) != 0; bx++){ 
          vx = sht->vx0 + bx;
          if(map[vy * ctl->xsize + vx] == sid){
            vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
          }
        }
        vx = sht->vx0 + bx;
        p = (int *) &map[vy * ctl->xsize + vx];
        q = (int *) &vram[vy * ctl->xsize + vx];
        r = (int *) &buf[by * sht->bxsize + bx];
        for(i=0;i<i1;i++){            
          if(p[i] == sid4){
            q[i] = r[i];
          }else{
            bx2 = bx + i * 4;
            vx = sht->vx0 + bx2;
            if(map[vy * ctl->xsize + vx + 0] == sid){
              vram[vy * ctl->xsize + vx + 0] = buf[by * sht->bxsize + bx2 + 0];
              if(by*sht->bxsize+bx2<0){
                vram[vy * ctl->xsize + vx + 0] = COL8_008484;
              }
            }
            if(map[vy * ctl->xsize + vx + 1] == sid){
              vram[vy * ctl->xsize + vx + 1] = buf[by * sht->bxsize + bx2 + 1];
              if(by*sht->bxsize+bx2<1){
                vram[vy * ctl->xsize + vx + 1] = COL8_008484;
              }
            }
            if(map[vy * ctl->xsize + vx + 2] == sid){
              vram[vy * ctl->xsize + vx + 2] = buf[by * sht->bxsize + bx2 + 2];
              if(by*sht->bxsize+bx2<2){
                vram[vy * ctl->xsize + vx + 2] = COL8_008484;
              }
            }
            if(map[vy * ctl->xsize + vx + 3] == sid){
              vram[vy * ctl->xsize + vx + 3] = buf[by * sht->bxsize + bx2 + 3];
              if(by*sht->bxsize+bx2<3){
                vram[vy * ctl->xsize + vx + 3] = COL8_008484;
              }
            }
          }
        }
        for (bx+=i1*4;bx<bx1;bx++){ 
          vx = sht->vx0 + bx;
          if(map[vy * ctl->xsize + vx] == sid){
            vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
          }
        }
      }
    }else{
      for(by = by0; by < by1; by++){
        vy = sht->vy0 + by;
        for(bx = bx0; bx < bx1; bx++){
          vx = sht->vx0 + bx;
          if(map[vy * ctl->xsize + vx] == sid){
            vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
          }
        }
      }
    }
  }
}
void sheet_refresh(SHEET *sht,int bx0,int by0,int bx1,int by1){
  SHTCTL *ctl = sht->ctl;
  if(sht->height>=0){
    sheet_refresh_sub(ctl,
        sht->vx0+bx0,sht->vy0+by0,sht->vx0+bx1,sht->vy0+by1,sht->height,sht->height);
  }
  return;
}
void sheet_slide( SHEET *sht, int vx0, int vy0){
   SHTCTL *ctl = sht->ctl;
  int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
  sht->vx0 = vx0;
  sht->vy0 = vy0;
  if (sht->height >= 0) {
    sheet_refresh_map(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0);
    sheet_refresh_map(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
    sheet_refresh_sub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0, sht->height - 1);
    sheet_refresh_sub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height, sht->height);
  }
  return;
}

void sheet_updown(SHEET *sht,int height){
  int h,old=sht->height;
  SHTCTL *ctl = sht->ctl;
  // 調整
  if(height > ctl->top + 1){
    height = ctl->top + 1;
  }
  if(height < -1){
    height = -1;
  }
  sht->height = height; 

  if(old > height){  // lowになる
    if(height >= 0){
      for(h=old;h>height;h--){
        ctl->sheets[h] = ctl->sheets[h - 1];
        ctl->sheets[h]->height = h;
      }
      ctl->sheets[height] = sht;
      sheet_refresh_map(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
      sheet_refresh_sub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old);
    }else{  // 消す
      if(ctl->top > old){
        for(h = old; h < ctl->top; h++){
          ctl->sheets[h] = ctl->sheets[h + 1];
          ctl->sheets[h]->height = h;
        }
      }
      ctl->top--; 
      sheet_refresh_map(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
      sheet_refresh_sub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0, old - 1);
    }
    //高くなる
  }else if(old<height){  
    if(old >= 0){
      for (h = old; h < height; h++) {
        ctl->sheets[h] = ctl->sheets[h + 1];
        ctl->sheets[h]->height = h;
      }
      ctl->sheets[height] = sht;
      //表示させる
    }else{  
      for(h = ctl->top; h >= height; h--){
        ctl->sheets[h + 1] = ctl->sheets[h];
        ctl->sheets[h + 1]->height = h + 1;
      }
      ctl->sheets[height] = sht;
      ctl->top++; 
    }
    sheet_refresh_map(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
    sheet_refresh_sub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height, height);
  }
  return;
}
void sheet_free(SHEET *sht){
  if(sht->height>=0){
    sheet_updown(sht,-1);
  }
  sht->flags=0;
 return;
}
