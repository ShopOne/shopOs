//definition

int max(int a,int b);
int min(int a,int b);
int abs(int a);
int strcmp(const char* s,const char *t);
int strn_cmp(const char* s,const char *t,int size);
int str_len(const char* s);
//keyboard.c
#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA 0x0064
#define PORT_KEYCMD 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE    0x60
#define KEYCMD_SENDTO_MOUSE  0xd4
#define MOUSECMD_ENABLE      0xf4
#define KBC_MODE    0x47
//graphic.c
#define COL8_000000    0
#define COL8_FF0000    1
#define COL8_00FF00    2
#define COL8_FFFF00    3
#define COL8_0000FF    4
#define COL8_FF00FF    5
#define COL8_00FFFF    6
#define COL8_FFFFFF    7
#define COL8_C6C6C6    8
#define COL8_840000    9
#define COL8_008400    10
#define COL8_848400    11
#define COL8_000084    12
#define COL8_840084    13
#define COL8_008484    14
#define COL8_848484    15
#define COL8_864815    16

//dsctbl.c
#define ADR_IDT      0x0026f800
#define LIMIT_IDT    0x000007ff
#define ADR_GDT      0x00270000
#define LIMIT_GDT    0x0000ffff
#define ADR_BOTPAK    0x00280000
#define LIMIT_BOTPAK  0x0007ffff
#define AR_DATA32_RW  0x4092
#define AR_CODE32_ER  0x409a
#define AR_INTGATE32  0x008e
#define AR_TSS32      0x0089
#define AR_LDT        0x0082
#define KEYBUF_STACKSIZE 32
#define MOUSEBUF_STACKSIZE 128
#define ADR_BOOTINFO 0x00000ff0
#define ADR_DISKIMG  0x00100000

//int.c
#define PIC0_ICW1    0x0020
#define PIC0_OCW2    0x0020
#define PIC0_IMR     0x0021
#define PIC0_ICW2    0x0021
#define PIC0_ICW3    0x0021
#define PIC0_ICW4    0x0021
#define PIC1_ICW1    0x00a0
#define PIC1_OCW2    0x00a0
#define PIC1_IMR     0x00a1
#define PIC1_ICW2    0x00a1
#define PIC1_ICW3    0x00a1
#define PIC1_ICW4    0x00a1
#define PORT_KEYDAT  0x0060

//fifo8_get
#define FLAGS_OVERRUN 0x0001

//memory
#define EFLAG_AC_BIT      0x00040000
#define CR0_CACHE_DISABLE 0x60000000
#define MEMMAN_FREES 4090
#define MEMMAN_ADDR 0x003c0000

//sheet
#define MAX_SHEETS 256
#define SHEET_USE 1
//timer
#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040
#define MAX_TIMER 50

//task 
#define MAX_TASKS   1000
#define TASK_GDT0   3
#define MAX_TASK_LV 100
#define MAX_TASKLEVELS 10
// structs
typedef struct {
  short limit_low, base_low;
  char base_mid, access_right;
  char limit_high, base_high;
}SEGMENT_DESCRIPTOR;

typedef struct{
  short offset_low, selector;
  char dw_count, access_right;
  short offset_high;
}GATE_DESCRIPTOR;

// asmhead.asm
typedef struct{
  char cyls,leds,vmode,reserve;
  short scrnx,scrny;
  char *vram;
}BOOTINFO;

typedef struct{
  unsigned char buf[3],phase;
  int x,y,btn;
}MOUSE_DEC;

typedef struct{
  unsigned int addr,size;
}FREEINFO;

typedef struct{
  int frees,maxfrees,lostsize,losts;
  FREEINFO free[MEMMAN_FREES];
}MEMMAN;

typedef struct TASK TASK;
struct SHEET;
struct SHTCTL;
typedef struct CONSOLE CONSOLE;
typedef struct SHEET SHEET;
typedef struct SHTCTL SHTCTL;
struct SHEET{
  unsigned char *buf;
  int bxsize,bysize,vx0,vy0,col_inv,height,flags;
  SHTCTL *ctl;
  TASK *task;
};
struct SHTCTL{
  unsigned char *vram,*map;
  int xsize,ysize,top;
  SHEET *sheets[MAX_SHEETS];
  SHEET sheets0[MAX_SHEETS];
};

typedef struct{
  //タスク設定
  int backlink,esp0,ss0,esp1,ss1,esp2,ss2,cr3;
  //32ビットレジスタ
  int eip,eflags,eax,ecx,edx,ebx,esp,ebp,esi,edi;
  //16ビットレジスタ
  int es,cs,ss,ds,fs,gs;
  //タスク設定
  int ldtr,iomap;
}TSS32;

typedef struct FIFO32 FIFO32;
struct FIFO32{
  int *buf;
  int p,q,size,free,flags;
  TASK *task;
};
typedef struct{
  char *buf;
  int size;
  int pos;
}FILEHANDLE;
struct TASK{
  int sel, flags;
  int level,priority;
  FIFO32 fifo;
  TSS32 tss;
  SEGMENT_DESCRIPTOR ldt[2];
  CONSOLE *cons;
  int ds_base,cons_stack;
  FILEHANDLE *fhandle;
  int *fat;
  char *cmdline;
  unsigned char langmode,langbyte1;
};
typedef struct{
  int running; //動作タスク数
  int now; //今動いているタスク管理
  TASK *tasks[MAX_TASK_LV];
}TASKLEVEL;

struct TIMER{
  struct TIMER *next;
  unsigned int timeout;
  char flags,flags2;
  FIFO32 *fifo;
  int data;
};
typedef struct TIMER TIMER;

typedef struct{
  unsigned int cnt,next,use;
  TIMER *t0;
  TIMER timers0[MAX_TIMER];
  
}TIMERCTL;

typedef struct{
  int now_lv;
  char lv_change;
  TASKLEVEL level[MAX_TASKLEVELS];
  TASK tasks0[MAX_TASKS];
}TASKCTL;
typedef struct {
  int size,clustno;
  unsigned char name[24];
}FILEINFO;

typedef struct{
  int size,addr;
  unsigned char name[24];
}FS_FILEINFO;
struct CONSOLE{
  SHEET *sht;
  int cur_x,cur_y,cur_c;
  TIMER *timer;
  char *cur_dir;
  unsigned int curdir_nsize;
};

// global
FIFO32 *key_fifo;
FIFO32 *mouse_fifo;
TIMERCTL timectl;
TASKCTL *taskctl;
TIMER *mt_timer;
TIMER *task_timer;
int mt_tr;
int keydata0;
int mousedata0;

//nasmfunc.asmhead
void io_hlt();
void io_cli();
void io_sti();
void io_stihlt();
void io_out8(int port,int data);
int io_in8(int port);
int io_load_eflags();
void io_store_eflags(int eflags);
void load_gdtr(int limit,int addr);
void load_idtr(int limit,int addr);
void asm_inthandler20();
void asm_inthandler21();
void asm_inthandler27();
void asm_inthandler2c();
void asm_inthandler0d();
void asm_inthandler0c();
void asm_end_app();
int load_cr0();
unsigned int memtest_sub(unsigned int start,unsigned int end);
void store_cr0(int cr0);
void load_tr(int tr);
void taskswitch4();
void taskswitch3();
void farjmp(int eip, int cs);
void farcall(int eip,int cs);
void asm_cons_putchar(void);
void asm_hrb_api();
void start_app(int eip,int cs,int esp,int da,int *tss_esp0);

//graphic.c
void init_palette();
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize,unsigned char c, int x0, int y0, int x1, int y1);
void init_screen8(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfont8_str(char *vram, int xsize, int x, int y, unsigned char c, unsigned char *s);
void putfont8_str_sht(SHEET *sht, int x,int y,int c,int b,char *s,int l);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
  int pysize, int px0, int py0, char *buf, int bxsize);

/* dsctbl.c */

void init_gdtidt(void);
void set_segmdesc(SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
/* int.c */
void init_pic(void);
void inthandler21(int *esp);
void inthandler27(int *esp);
void inthandler2c(int *esp);
//keyboard.c
void wait_KBC_sendready();
void init_keyboard(FIFO32 *fifo,int data0);
void inthandler21(int *esp);
void inthandler2c(int *esp);
//mouse.c
void enable_mouse(FIFO32 *fifo,int data0,MOUSE_DEC *mdec);
int mouse_decode(MOUSE_DEC *mdec,unsigned char dat);
// mysprintf
void my_sprintf (char *str, char *fmt, ...); 

//fifo8
void fifo32_init(FIFO32 *fifo,int size,int *buf,TASK *task);
int fifo32_put(FIFO32 *fifo, int  data);
int fifo32_get(FIFO32 *fifo);
int fifo32_statue(FIFO32 *fifo);
//memory.c
unsigned int memtest(unsigned int start,unsigned int end);
void memman_init(MEMMAN *man);
int memman_free(MEMMAN *man,unsigned int addr, unsigned int size);
unsigned int memman_total(MEMMAN *man);
unsigned int memman_alloc(MEMMAN *man,unsigned int size);
unsigned int memman_alloc_4k(MEMMAN *man,unsigned int size);
int memman_free_4k(MEMMAN *man,unsigned int addr, unsigned int size);
int memman_free(MEMMAN *man,unsigned int addr,unsigned int size);
//sheet.c
SHTCTL * shtctl_init(MEMMAN *memman,unsigned char *vram,int xsize,int ysize);
SHEET *sheet_alloc(SHTCTL *ctl);
void sheet_setbuf(SHEET* sht, unsigned char *buf, int xsize, int ysize,int col_inv);
void sheet_updown(SHEET *sht, int height);
void sheet_refresh(SHEET *sht,int bx0,int by0,int bx1,int by1);
void sheet_slide(SHEET *sht,int vx0,int vy0);
void sheet_free(SHEET *sht);
// timer.c
void init_pit();
void inthandler20(int *esp);
void timer_settime(TIMER *timer,unsigned int timeout);
void timer_init(TIMER *timer, FIFO32 *fifo, int data);
void timer_cancelall(FIFO32 *fifo);
int  timer_cancel(TIMER* timer);
void timer_free(TIMER *timer);
TIMER *timer_alloc();

// mtask
void mt_init();
void mt_taskswitch();
void task_switch();
void task_run(TASK *task,int level,int priority);
void task_sleep(TASK *task);
TASK *task_alloc();
TASK *task_init();
TASK *task_now();
//window
void change_wtitle8(SHEET *sht,char act);
void make_window8(unsigned char *buf,int size,int ysize,char *title,char act);
void make_textbox8(SHEET *sht,int x0,int y0,int sx,int sy,int c);
void make_wtitle8(unsigned char *buf,int xsize,char *title,char act);
//console_task
void console_task(SHEET *sheet, unsigned int memtotal);
void cons_putchar(CONSOLE *cons,int chr,int move);
void cons_putstr0(CONSOLE *cons,const char *s);
int *hrb_api(int edi,int esi,int ebp,int esp,int ebx,int edx,int esc,int eax);
int *inthandler0d(int *esp);
int *inthandler0c(int *esp);
//file 
FILEINFO *file_search(char *name,FILEINFO *finfo,int max);
void file_readfat(int *fat,unsigned char *img);
void file_loadfile(int clustno,int size,char *buf,char *img);
char *file_loadfile2(int clustno, int *psize);
//bootpack
SHEET *open_console(SHTCTL *shtctl,unsigned int memtotal);
TASK *open_constask(SHEET *sht,unsigned int memtotal);
