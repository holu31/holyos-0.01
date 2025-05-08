.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	push %ebx
	call kmain
	
	cli
1:	hlt
	jmp 1b

.global init_ctors
init_ctors:
	movl $__ctors_start, %ebx
	jmp 1f
0:
	call *(%ebx)
	add $4, %ebx
1:
	cmpl $__ctors_end, %ebx
	jb 0b
	ret

.size _start, . - _start
