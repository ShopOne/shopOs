#include"bootpack.h"
void fifo32_init(FIFO32 *fifo,int size,int *buf,TASK *task){
  fifo->size=size;
  fifo->buf=buf;
  fifo->free=size; //free space
  fifo->flags=0;
  fifo->p=0;//start
  fifo->q=0;//end
  fifo->task = task;
  return;
}
int fifo32_put(FIFO32 *fifo,int data){
  if(fifo->free==0){
    fifo->flags |= FLAGS_OVERRUN;
    return -1;
  }
  fifo->buf[fifo->p++]=data;
  fifo->p%=fifo->size;
  fifo->free--;
  if(fifo->task!=0){
    if(fifo->task->flags != 2){
      task_run(fifo->task,-1,0);
    }
  }
  return 0;
}
int fifo32_get(FIFO32 *fifo){
  int data;
  if(fifo->free==fifo->size)return -1;
  data = fifo->buf[fifo->q++];
  fifo->q%=fifo->size;
  fifo->free++;
  return data;
}
int fifo32_statue(FIFO32 *fifo){
  return fifo->size-fifo->free;
}
