#include"bootpack.h"
void dmac_task(){
  // write job
}
void init_fdctask(){
  MEMMAN *memman = (MEMMAN *) MEMMAN_ADDR;
  TASK *task = task_alloc();
  int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
  task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
  task->tss.esp = task->cons_stack + 64 * 1024 - 12;
  task->tss.eip = (int) &dmac_task;
  task->tss.es = 1 * 8;
  task->tss.cs = 2 * 8;
  task->tss.ss = 1 * 8;
  task->tss.ds = 1 * 8;
  task->tss.fs = 1 * 8;
  task->tss.gs = 1 * 8;
  /*
  *((int *) (task->tss.esp + 4)) = (int) sht;
  *((int *) (task->tss.esp + 8)) = memtotal;
  */
  task_run(task, 2, 2); 
  fifo32_init(&task->fifo, 128, cons_fifo, task);
}
void init_dmac(){
  io_out8(0x00d6,0xc0);
  io_out8(0x00c0,0x00);
  io_out8(0x000a,0x06);
}
void inthandler26(int *esp){
  io_in8(0x03f4);
  io_out8(PIC0_OCW2,0x66);
  fifo32_put(fdc_fifo,3);
}
