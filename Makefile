#ファイル生成規則
API_PATH = ./api/
OBJ_PATH = ./obj/
API_SRC  = $(wildcard ./api/*.asm)

OBJECTS = $(subst $(API_PATH),$(OBJ_PATH),$(API_SRC:.asm=.o))


default :
	make img

ipl10.bin : ipl10.asm Makefile
	nasm ipl10.asm -o ipl10.bin -l ipl10.lst

convHankakuTxt : convHankakuTxt.c
	$(CC) $< -o convHankakuTxt

hankaku.c : hankaku.txt convHankakuTxt
	./convHankakuTxt
asmhead.bin : asmhead.asm Makefile
	nasm asmhead.asm -o asmhead.bin -l asmhead.lst

nasmfunc.o : nasmfunc.asm Makefile
	nasm -g -f elf nasmfunc.asm -o nasmfunc.o

apilib.a: $(OBJECTS) Makefile
	ar r apilib.a $(OBJECTS)
	ar q apilib.a obj/bmp.obj

%.hrb : %.o apilib.a Makefile 
	ld -m elf_i386 -e HariMain -o $@ -Tapi.ls $<  apilib.a

%.o: %.c Makefile 
	$(CC) -fno-pic -march=i486 -m32  -nostdlib $< -c

%.o: %.asm Makefile
	nasm -f elf32 -o $@ $<

./obj/%.o: $(API_SRC) Makefile
	nasm -f elf32 -o $@ $(subst obj/,api/,$(@:.o=.asm))

bootpack.hrb : bootpack.o dsctbl.o keyboard.o mtask.o template_func.o memory.o sheet.o mouse.o graphic.o timer.o int.o fifo.o window.o console.o file.o hankaku.c mysprintf.c bootpack.h har.ld nasmfunc.o Makefile
	gcc -fno-pic -march=i486 -m32 -nostdlib -T  har.ld -g hankaku.c graphic.o template_func.o dsctbl.o fifo.o mysprintf.c bootpack.o nasmfunc.o memory.o console.o file.o window.o mtask.o mouse.o keyboard.o sheet.o timer.o int.o -o bootpack.hrb

haribote.sys : asmhead.bin bootpack.hrb  Makefile
	cat asmhead.bin bootpack.hrb > haribote.sys


haribote.img : ipl10.bin haribote.sys nihongo.fnt typeipl.hrb cat.hrb lines.hrb  ipl10.asm euc.txt calc.hrb notrec.hrb bball.hrb invad.hrb Makefile
	mformat -f 1440 -C -B $< -i $@ ::
	mcopy haribote.sys -i $@ ::
	mcopy Makefile -i $@ ::
	mcopy ipl10.bin -i $@ ::
	mcopy cat.hrb -i $@ ::
	mcopy lines.hrb -i $@ ::
	mcopy nihongo.fnt -i $@ ::
	mcopy bball.hrb -i $@ ::
	mcopy notrec.hrb -i $@ ::
	mcopy ipl10.asm -i $@ ::
	mcopy invad.hrb -i $@ ::
	mcopy calc.hrb -i $@ ::
	mcopy euc.txt -i $@ ::


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
	rm *.lst *.bin *.sys *.img *.hrb *.o *.a ./obj/*.o

#japanese にほんご日本語です
