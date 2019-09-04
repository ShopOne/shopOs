#include"bootpack.h"
void task_idle(void){
  while(1){
    io_hlt();
  }
}

void task_add(TASK *task){
  TASKLEVEL *tl = &taskctl->level[task->level];
  tl->tasks[tl->running++] = task;
  task->flags = 2;
  return;
}
TASK *task_now(){
  TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
  return tl->tasks[tl->now];
}
void task_remove(TASK *task){
  int i;
  TASKLEVEL *tl;
  tl = &taskctl->level[task->level];
  for(i=0;i<tl->running;i++){
    if(tl->tasks[i]==task){
      break;
    }
  }
  tl->running--;
  //今あるやつ減る
  if(i < tl->now){
    tl->now--;
  }
  if(tl->now >= tl->running){
    tl->now=0;
  }
  task->flags = 1;

  for(;i<tl->running;i++){
    tl->tasks[i] = tl->tasks[i+1];
  }
  return;
}
void task_switchsub(){
  int i;
  for(i=0;i<MAX_TASKLEVELS;i++){
    if(taskctl->level[i].running>0){
      break;
    }
  }
  taskctl->now_lv = i;
  taskctl->lv_change = 0;
  return;
}
void task_sleep(TASK *task){
  TASK *now_task;
  if (task->flags == 2) {
    now_task = task_now();
    task_remove(task);
    if(task==now_task){
      task_switchsub();
      now_task = task_now();
      farjmp(0, now_task->sel);
    }
  }
  return;
}
void task_switch(){
  TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
  TASK *new_task,*now_task=tl->tasks[tl->now];
  tl->now++;
  if(tl->now == tl->running){
    tl->now=0;
  }
  //レベルに変更がございませんでした
  if(taskctl->lv_change!=0){
    task_switchsub();
    tl = &taskctl->level[taskctl->now_lv];
  }
  new_task = tl->tasks[tl->now];
  timer_settime(task_timer, new_task->priority);
  if(new_task != now_task){
    farjmp(0, new_task->sel);
  }
  return;
}
void task_run(TASK *task,int level,int priority){
  if(level<0){
    level = task->level;
  }
  if(priority>0){
    task->priority = priority;
  }
  // 動作中のものをレベル変更
  if(task->flags==2 && task->level!=level){
    //removeをするときに、flagは1となるので、結局レベル設定される
    task_remove(task);
  }
  //タスク追加、ついでにレベルも設定
  if(task->flags!=2){
    task->level = level;
    task_add(task);
  }
  //あとでチェックするため
  taskctl->lv_change = 1;
  return;
}
TASK *task_alloc(){
  TASK *task;
  for(int i=0;i<MAX_TASKS;i++){
    //初期設定をとりあえず、困るなら後で設定
    if(taskctl->tasks0[i].flags==0){
      task = &taskctl->tasks0[i];
      task->flags = 1;
      task->tss.eflags = 0x00000202;
      task->tss.eax = 0;
      task->tss.ecx = 0;
      task->tss.edx = 0;
      task->tss.ebx = 0;
      task->tss.ebp = 0;
      task->tss.esi = 0;
      task->tss.edi = 0;
      task->tss.es = 0;
      task->tss.ds = 0;
      task->tss.fs = 0;
      task->tss.gs = 0;
      task->tss.iomap = 0x40000000;
      task->tss.ss0 = 0;
      return task;
    }
  }
  return 0;
}
TASK *task_init(MEMMAN *memman){
  TASK *task,*idle;
  SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR*)ADR_GDT;
  taskctl = (TASKCTL*) memman_alloc_4k(memman,sizeof(TASKCTL));
  for(int i=0;i<MAX_TASKS;i++){
    taskctl->tasks0[i].flags = 0;
    taskctl->tasks0[i].sel = (TASK_GDT0 + i)*8;
    taskctl->tasks0[i].tss.ldtr =(TASK_GDT0+MAX_TASKS+i)*8;
    set_segmdesc(gdt+TASK_GDT0+i, 103,
        (int)&taskctl->tasks0[i].tss, AR_TSS32);
    set_segmdesc(gdt+TASK_GDT0+MAX_TASKS+i, 15,
        (int)taskctl->tasks0[i].ldt, AR_LDT);
  }
  for(int i=0;i<MAX_TASKLEVELS;i++){
    taskctl->level[i].running = 0;
    taskctl->level[i].now = 0;
  }
  task = task_alloc();
  // 動かして、priorityは0.02秒、レベルは最高が初期設定
  task->flags = 2;
  task->priority = 2;
  task->level = 0;
  task_add(task);
  task_switchsub();
  load_tr(task->sel);
  task_timer=timer_alloc();
  timer_settime(task_timer,task->priority);

  idle = task_alloc();
  idle->tss.esp = memman_alloc_4k(memman,64*1024) + 64*1024;
  idle->tss.eip = (int)&task_idle;
  idle->tss.es = 1*8;
  idle->tss.cs = 2*8;
  idle->tss.ss = 1*8;
  idle->tss.ds = 1*8;
  idle->tss.fs = 1*8;
  idle->tss.gs = 1*8;
  task_run(idle,MAX_TASKLEVELS-1,1);
  return task;
}
