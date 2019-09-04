#include"bootpack.h"
#define TIMER_FLAGS_NOUSE 0
#define TIMER_FLAGS_ALLOC 1
#define TIMER_FLAGS_USING 2
int timer_cancel(TIMER *timer){
  int e;
  TIMER *t;
  e = io_load_eflags();
  io_cli();
  if(timer->flags==TIMER_FLAGS_USING){
    if(timer==timectl.t0){
      t = timer->next;
      timectl.t0 = t;
      timectl.next = t->timeout;
    }else{
      t = timectl.t0;
      while(1){
        if(t->next==timer){
          break;
        }
        t = t->next;
      }
      t->next = timer->next;
    }
    timer->flags = TIMER_FLAGS_ALLOC;
    io_store_eflags(e);
    return 1;
  }
  io_store_eflags(e);
  return 0;
}
void timer_cancelall(FIFO32 *fifo){
  int e;
  TIMER *t;
  e = io_load_eflags();
  io_cli();
  for(int i=0;i<MAX_TIMER;i++){
    t = &timectl.timers0[i];
    if(t->flags!=0&&t->flags2!=0&&t->fifo==fifo){
      timer_cancel(t);
      timer_free(t);
    }
  }
  io_store_eflags(e);
  return;
}
void inthandler20(int *esp){
  TIMER *timer;
  char ts = 0;
  io_out8(PIC0_OCW2,0x60);
  timectl.cnt++;
  if(timectl.next > timectl.cnt) return;
  timer = timectl.t0;
  while(1){
    if(timer->timeout > timectl.cnt) break;
    timer->flags = TIMER_FLAGS_ALLOC;
    if(timer != task_timer){
      fifo32_put(timer->fifo, timer->data);
    }else{
      ts = 1;
    }
    timer = timer->next;
  }
  timectl.t0=timer;
  timectl.next = timer->timeout;
  if(ts != 0){
    task_switch();
  }
  return;
}
TIMER *timer_alloc(){
  for(int i=0;i<MAX_TIMER;i++){
    if(timectl.timers0[i].flags==TIMER_FLAGS_NOUSE){
      timectl.timers0[i].flags=TIMER_FLAGS_ALLOC;
      timectl.timers0[i].flags2 = 0;
      return &timectl.timers0[i];
    }
  }
  return 0;
}
void timer_free(TIMER *timer){
  timer->flags=0;
}
void timer_settime(TIMER *timer,unsigned int timeout){
  int e;
  TIMER *t,*s;
  timer->timeout = timeout + timectl.cnt;
  timer->flags = TIMER_FLAGS_USING;
  e = io_load_eflags();
  io_cli();
  t = timectl.t0;
  //先頭
  if(timer->timeout <= t->timeout){
    timectl.t0 = timer;
    timer->next = t;
    timectl.next = timer->timeout;
    io_store_eflags(e);
    return;
  }
  while(1){
    s=t;
    t=t->next;
    if(t==0) break;
    //sとtのあいだへ
    if(timer->timeout <= t->timeout){
      s->next = timer;
      timer->next = t;
      io_store_eflags(e);
      return;
    }
  }
}
void timer_init(TIMER *timer, FIFO32 *fifo, int data){
  timer->fifo = fifo;
  timer->data = data;
  return;
}
void init_pit(){
  TIMER *t;
  io_out8(PIT_CTRL,0x34);
  io_out8(PIT_CNT0,0x9c);
  io_out8(PIT_CNT0,0x2e);
  timectl.cnt=0;
  timectl.next=0xfffffff;
  for(int i=0;i<MAX_TIMER;i++){
    timectl.timers0[i].flags=TIMER_FLAGS_NOUSE;
  }
  t = timer_alloc();
  t->timeout = 0xfffffff;
  t->flags = TIMER_FLAGS_USING;
  t->next = 0;
  timectl.t0 = t;
  timectl.next = 0xfffffff;
  return;
}
