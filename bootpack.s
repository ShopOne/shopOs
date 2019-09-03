	.file	"bootpack.c"
	.text
	.comm	key_fifo,32,32
	.comm	mouse_fifo,32,32
	.globl	memtest_sub
	.type	memtest_sub, @function
memtest_sub:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -36(%rbp)
	movl	%esi, -40(%rbp)
	movl	$-1437226411, -16(%rbp)
	movl	$1437226410, -12(%rbp)
	movl	-36(%rbp), %eax
	movl	%eax, -24(%rbp)
	jmp	.L2
.L7:
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -20(%rbp)
	movl	-16(%rbp), %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	notl	%eax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	movq	-8(%rbp), %rax
	movl	(%rax), %edx
	movl	-12(%rbp), %eax
	cmpl	%eax, %edx
	je	.L3
	nop
	jmp	.L4
.L9:
	nop
.L4:
	movl	-20(%rbp), %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	jmp	.L5
.L3:
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	notl	%eax
	movl	%eax, %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	movq	-8(%rbp), %rax
	movl	(%rax), %edx
	movl	-16(%rbp), %eax
	cmpl	%eax, %edx
	jne	.L9
	movl	-20(%rbp), %edx
	movq	-8(%rbp), %rax
	movl	%edx, (%rax)
	movl	-24(%rbp), %eax
	addl	$4096, %eax
	movl	%eax, -24(%rbp)
.L2:
	movl	-24(%rbp), %eax
	cmpl	%eax, -40(%rbp)
	jnb	.L7
.L5:
	movl	-24(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	memtest_sub, .-memtest_sub
	.globl	memtest
	.type	memtest, @function
memtest:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movb	$0, -13(%rbp)
	movl	$0, %eax
	call	io_load_eflags@PLT
	movl	%eax, -12(%rbp)
	orl	$262144, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	io_store_eflags@PLT
	movl	$0, %eax
	call	io_load_eflags@PLT
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	andl	$262144, %eax
	testl	%eax, %eax
	je	.L11
	movb	$1, -13(%rbp)
.L11:
	andl	$-262145, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	io_store_eflags@PLT
	cmpb	$0, -13(%rbp)
	je	.L12
	movl	$0, %eax
	call	load_cr0@PLT
	movl	%eax, -8(%rbp)
	orl	$1610612736, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, %edi
	call	store_cr0@PLT
.L12:
	movl	-24(%rbp), %edx
	movl	-20(%rbp), %eax
	movl	%edx, %esi
	movl	%eax, %edi
	call	memtest_sub
	movl	%eax, -4(%rbp)
	cmpb	$0, -13(%rbp)
	je	.L13
	movl	$0, %eax
	call	load_cr0@PLT
	movl	%eax, -8(%rbp)
	andl	$-1610612737, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, %edi
	call	store_cr0@PLT
.L13:
	movl	-4(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	memtest, .-memtest
	.section	.rodata
.LC0:
	.string	"memory %dMB"
.LC1:
	.string	"%x"
.LC2:
	.string	"lrc %d %d"
.LC3:
	.string	"(%d, %d)"
	.text
	.globl	HariMain
	.type	HariMain, @function
HariMain:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$408, %rsp
	.cfi_offset 3, -24
	movq	%fs:40, %rax
	movq	%rax, -24(%rbp)
	xorl	%eax, %eax
	movq	$4080, -392(%rbp)
	call	init_gdtidt@PLT
	call	init_pic@PLT
	movl	$0, %eax
	call	io_sti@PLT
	movl	$0, %eax
	call	init_keyboard@PLT
	leaq	-384(%rbp), %rax
	movq	%rax, %rdi
	call	enable_mouse@PLT
	call	init_palette@PLT
	movq	-392(%rbp), %rax
	movzwl	6(%rax), %eax
	movswl	%ax, %edx
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	movswl	%ax, %ecx
	movq	-392(%rbp), %rax
	movq	8(%rax), %rax
	movl	%ecx, %esi
	movq	%rax, %rdi
	call	init_screen8@PLT
	leaq	-368(%rbp), %rax
	movq	%rax, %rdx
	movl	$32, %esi
	leaq	key_fifo(%rip), %rdi
	call	fifo8_init@PLT
	leaq	-368(%rbp), %rax
	movq	%rax, %rdx
	movl	$32, %esi
	leaq	mouse_fifo(%rip), %rdi
	call	fifo8_init@PLT
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	cwtl
	subl	$16, %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	movl	%eax, -408(%rbp)
	movq	-392(%rbp), %rax
	movzwl	6(%rax), %eax
	cwtl
	subl	$44, %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	movl	%eax, -404(%rbp)
	leaq	-288(%rbp), %rax
	movl	$14, %esi
	movq	%rax, %rdi
	call	init_mouse_cursor8@PLT
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	movswl	%ax, %esi
	movq	-392(%rbp), %rax
	movq	8(%rax), %rax
	movl	-404(%rbp), %edi
	movl	-408(%rbp), %ecx
	pushq	$16
	leaq	-288(%rbp), %rdx
	pushq	%rdx
	movl	%edi, %r9d
	movl	%ecx, %r8d
	movl	$16, %ecx
	movl	$16, %edx
	movq	%rax, %rdi
	call	putblock8_8@PLT
	addq	$16, %rsp
	movl	$249, %esi
	movl	$33, %edi
	call	io_out8@PLT
	movl	$239, %esi
	movl	$161, %edi
	call	io_out8@PLT
	movl	$-1073741825, %esi
	movl	$262144, %edi
	call	memtest
	shrl	$20, %eax
	movl	%eax, -400(%rbp)
	movl	-400(%rbp), %edx
	leaq	-336(%rbp), %rax
	leaq	.LC0(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	my_sprintf@PLT
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	movswl	%ax, %esi
	movq	-392(%rbp), %rax
	movq	8(%rax), %rax
	leaq	-336(%rbp), %rdx
	movq	%rdx, %r9
	movl	$7, %r8d
	movl	$32, %ecx
	movl	$0, %edx
	movq	%rax, %rdi
	call	putfont8_str@PLT
.L26:
	movl	$0, %eax
	call	io_cli@PLT
	leaq	key_fifo(%rip), %rdi
	call	fifo8_statue@PLT
	movl	%eax, %ebx
	leaq	mouse_fifo(%rip), %rdi
	call	fifo8_statue@PLT
	addl	%ebx, %eax
	testl	%eax, %eax
	jne	.L16
	movl	$0, %eax
	call	io_stihlt@PLT
	jmp	.L26
.L16:
	leaq	key_fifo(%rip), %rdi
	call	fifo8_statue@PLT
	testl	%eax, %eax
	je	.L18
	leaq	key_fifo(%rip), %rdi
	call	fifo8_get@PLT
	movl	%eax, -396(%rbp)
	movl	$0, %eax
	call	io_sti@PLT
	movl	-396(%rbp), %edx
	leaq	-336(%rbp), %rax
	leaq	.LC1(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	my_sprintf@PLT
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	movswl	%ax, %esi
	movq	-392(%rbp), %rax
	movq	8(%rax), %rax
	subq	$8, %rsp
	pushq	$31
	movl	$15, %r9d
	movl	$16, %r8d
	movl	$0, %ecx
	movl	$14, %edx
	movq	%rax, %rdi
	call	boxfill8@PLT
	addq	$16, %rsp
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	movswl	%ax, %esi
	movq	-392(%rbp), %rax
	movq	8(%rax), %rax
	leaq	-336(%rbp), %rdx
	movq	%rdx, %r9
	movl	$7, %r8d
	movl	$16, %ecx
	movl	$0, %edx
	movq	%rax, %rdi
	call	putfont8_str@PLT
	jmp	.L26
.L18:
	leaq	mouse_fifo(%rip), %rdi
	call	fifo8_statue@PLT
	testl	%eax, %eax
	je	.L26
	leaq	mouse_fifo(%rip), %rdi
	call	fifo8_get@PLT
	movl	%eax, -396(%rbp)
	movl	$0, %eax
	call	io_sti@PLT
	movl	-396(%rbp), %eax
	movzbl	%al, %edx
	leaq	-384(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %rdi
	call	mouse_decode@PLT
	testl	%eax, %eax
	je	.L26
	movl	-376(%rbp), %ecx
	movl	-380(%rbp), %edx
	leaq	-336(%rbp), %rax
	leaq	.LC2(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	my_sprintf@PLT
	movl	-372(%rbp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L19
	movb	$76, -336(%rbp)
.L19:
	movl	-372(%rbp), %eax
	andl	$2, %eax
	testl	%eax, %eax
	je	.L20
	movb	$82, -334(%rbp)
.L20:
	movl	-372(%rbp), %eax
	andl	$4, %eax
	testl	%eax, %eax
	je	.L21
	movb	$67, -335(%rbp)
.L21:
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	movswl	%ax, %esi
	movq	-392(%rbp), %rax
	movq	8(%rax), %rax
	subq	$8, %rsp
	pushq	$31
	movl	$151, %r9d
	movl	$16, %r8d
	movl	$32, %ecx
	movl	$14, %edx
	movq	%rax, %rdi
	call	boxfill8@PLT
	addq	$16, %rsp
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	movswl	%ax, %esi
	movq	-392(%rbp), %rax
	movq	8(%rax), %rax
	leaq	-336(%rbp), %rdx
	movq	%rdx, %r9
	movl	$7, %r8d
	movl	$16, %ecx
	movl	$32, %edx
	movq	%rax, %rdi
	call	putfont8_str@PLT
	movl	-404(%rbp), %eax
	leal	15(%rax), %edi
	movl	-408(%rbp), %eax
	leal	15(%rax), %r8d
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	movswl	%ax, %esi
	movq	-392(%rbp), %rax
	movq	8(%rax), %rax
	movl	-404(%rbp), %ecx
	movl	-408(%rbp), %edx
	subq	$8, %rsp
	pushq	%rdi
	movl	%r8d, %r9d
	movl	%ecx, %r8d
	movl	%edx, %ecx
	movl	$14, %edx
	movq	%rax, %rdi
	call	boxfill8@PLT
	addq	$16, %rsp
	movl	-380(%rbp), %eax
	addl	%eax, -408(%rbp)
	movl	-376(%rbp), %eax
	addl	%eax, -404(%rbp)
	cmpl	$0, -408(%rbp)
	jns	.L22
	movl	$0, -408(%rbp)
.L22:
	cmpl	$0, -404(%rbp)
	jns	.L23
	movl	$0, -404(%rbp)
.L23:
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	cwtl
	subl	$16, %eax
	cmpl	%eax, -408(%rbp)
	jle	.L24
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	cwtl
	subl	$16, %eax
	movl	%eax, -408(%rbp)
.L24:
	movq	-392(%rbp), %rax
	movzwl	6(%rax), %eax
	cwtl
	subl	$16, %eax
	cmpl	%eax, -404(%rbp)
	jle	.L25
	movq	-392(%rbp), %rax
	movzwl	6(%rax), %eax
	cwtl
	subl	$16, %eax
	movl	%eax, -404(%rbp)
.L25:
	movl	-404(%rbp), %ecx
	movl	-408(%rbp), %edx
	leaq	-336(%rbp), %rax
	leaq	.LC3(%rip), %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	my_sprintf@PLT
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	movswl	%ax, %esi
	movq	-392(%rbp), %rax
	movq	8(%rax), %rax
	subq	$8, %rsp
	pushq	$15
	movl	$79, %r9d
	movl	$0, %r8d
	movl	$0, %ecx
	movl	$14, %edx
	movq	%rax, %rdi
	call	boxfill8@PLT
	addq	$16, %rsp
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	movswl	%ax, %esi
	movq	-392(%rbp), %rax
	movq	8(%rax), %rax
	leaq	-336(%rbp), %rdx
	movq	%rdx, %r9
	movl	$7, %r8d
	movl	$0, %ecx
	movl	$0, %edx
	movq	%rax, %rdi
	call	putfont8_str@PLT
	movq	-392(%rbp), %rax
	movzwl	4(%rax), %eax
	movswl	%ax, %esi
	movq	-392(%rbp), %rax
	movq	8(%rax), %rax
	movl	-404(%rbp), %edi
	movl	-408(%rbp), %ecx
	pushq	$16
	leaq	-288(%rbp), %rdx
	pushq	%rdx
	movl	%edi, %r9d
	movl	%ecx, %r8d
	movl	$16, %ecx
	movl	$16, %edx
	movq	%rax, %rdi
	call	putblock8_8@PLT
	addq	$16, %rsp
	jmp	.L26
	.cfi_endproc
.LFE2:
	.size	HariMain, .-HariMain
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
