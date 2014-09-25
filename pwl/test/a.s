	.file	"a.i"
	.text
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp1:
	.cfi_def_cfa_offset 16
	movl	$256, %edi              # imm = 0x100
	callq	malloc
	movq	%rax, g_buff(%rip)
	movq	%rax, %rdi
	callq	free
	movl	$256, %edi              # imm = 0x100
	callq	malloc
	movq	%rax, g_buff+8(%rip)
	movq	%rax, %rdi
	callq	free
	movl	$256, %edi              # imm = 0x100
	callq	malloc
	movq	%rax, g_buff+16(%rip)
	movq	%rax, %rdi
	callq	free
	movl	$256, %edi              # imm = 0x100
	callq	malloc
	movq	%rax, g_buff+24(%rip)
	movq	%rax, %rdi
	callq	free
	movl	$256, %edi              # imm = 0x100
	callq	malloc
	movq	%rax, g_buff+32(%rip)
	movq	%rax, %rdi
	callq	free
	movl	$256, %edi              # imm = 0x100
	callq	malloc
	movq	%rax, g_buff+40(%rip)
	movq	%rax, %rdi
	callq	free
	movl	$256, %edi              # imm = 0x100
	callq	malloc
	movq	%rax, g_buff+48(%rip)
	movq	%rax, %rdi
	callq	free
	movl	$256, %edi              # imm = 0x100
	callq	malloc
	movq	%rax, g_buff+56(%rip)
	movq	%rax, %rdi
	callq	free
	movl	$256, %edi              # imm = 0x100
	callq	malloc
	movq	%rax, g_buff+64(%rip)
	movq	%rax, %rdi
	callq	free
	movl	$256, %edi              # imm = 0x100
	callq	malloc
	movq	%rax, g_buff+72(%rip)
	movq	%rax, %rdi
	callq	free
	xorl	%eax, %eax
	popq	%rdx
	ret
.Ltmp2:
	.size	main, .Ltmp2-main
	.cfi_endproc

	.type	g_buff,@object          # @g_buff
	.comm	g_buff,80,16

	.ident	"clang version 3.4 (tags/RELEASE_34/final)"
	.section	".note.GNU-stack","",@progbits
