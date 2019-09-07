#ファイル生成規則
API_PATH = ./api/
HARI_PATH = ./haribote/
API_SRC  = $(wildcard ./api/*.asm)
HARI_PACK = graphic.c template_func.c dsctbl.c fifo.c  bootpack.c nasmfunc.c memory.c console.c file.c window.c mtask.c mouse.c keyboard.c sheet.c timer.c int.c
HARI_SRC = $(addprefix $(HARI_PATH),$(HARI_PACK))
HARI_OBJ = $(HARI_SRC:.c=.o)
API_OBJ = $(API_SRC:.asm=.o)

default :
	make img

check:
	echo $(API_OBJ)

ipl10.bin : ipl10.asm Makefile
	nasm ipl10.asm -o ipl10.bin -l ipl10.lst

convHankakuTxt : convHankakuTxt.c
	$(CC) $< -o convHankakuTxt

hankaku.c : hankaku.txt convHankakuTxt
	./convHankakuTxt
asmhead.bin : asmhead.asm Makefile
	nasm asmhead.asm -o asmhead.bin -l asmhead.lst

$(HARI_PATH)%.o: $(HARI_PATH)%.c Makefile 
	$(CC) -fno-pic -march=i486 -m32  -nostdlib $< -c  -o $@

$(API_PATH)%.o: %.c Makefile 
	$(CC) -fno-pic -march=i486 -m32  -nostdlib $< -c  -o $@

$fs_img.out: fs_img.c Makefile 
	$(CC) -fno-pic -march=i486 -m32  -nostdlib $< -c  -o $@

$(HARI_PATH)nasmfunc.o : $(HARI_PATH)nasmfunc.asm Makefile
	nasm -g -f elf $(HARI_PATH)nasmfunc.asm -o $@

apilib.a: $(API_OBJ) Makefile
	ar r apilib.a $(API_OBJ)
	ar q apilib.a $(API_PATH)bmp.obj

%.hrb :$(API_PATH)%.o apilib.a Makefile 
	ld -m elf_i386 -e HariMain -o $@ -Tapi.ls $<  apilib.a

%.o: %.asm Makefile
	nasm -f elf32 -o $@ $<

$(API_PATH)%.o: $(API_PATH)%.asm Makefile
	nasm -f elf32 -o $@ $<

bootpack.hrb : $(HARI_OBJ) hankaku.c mysprintf.c har.ld $(HARI_PATH)nasmfunc.o Makefile
	gcc -fno-pic -march=i486 -m32 -nostdlib -T har.ld -g hankaku.c mysprintf.c $(HARI_OBJ) -o bootpack.hrb

haribote.sys : asmhead.bin bootpack.hrb  Makefile
	cat asmhead.bin bootpack.hrb > haribote.sys

fs_img.out: fs_img.c
	gcc fs_img.c -o fs_img.out

haribote.img : ipl10.bin haribote.sys fs_img.out
	./fs_img.out haribote.img ipl10.bin haribote.sys


asm :
	make -r ipl10.bin

img :
	make -r haribote.img

run :
	make img
	qemu-system-i386 -fda haribote.img

debug :
	make img
	qemu-system-i386 -fda  haribote.img -gdb tcp::10000 -S 

clean :
	rm *.lst *.bin *.sys *.img *.hrb *.a ./api/*.o ./haribote/*.o

