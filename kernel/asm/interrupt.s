.section .text

.macro ISR_NOERR num
	.global isr_stub_\num
	isr_stub_\num:
		cli
		pushl $0
		pushl $\num
		jmp interrupt_common_stub
.endm

.macro ISR_ERR num
	.global isr_stub_\num
	isr_stub_\num:
		cli
		pushl $\num
		jmp interrupt_common_stub
.endm

.macro IRQ num
	.global irq_stub_\num
	irq_stub_\num:
		cli
		pushl $0
		pushl $\num + 32
		jmp interrupt_common_stub
.endm

ISR_NOERR 0		# Divide Error
ISR_NOERR 1		# Debug
ISR_NOERR 2		# NMI
ISR_NOERR 3		# Breakpoint
ISR_NOERR 4		# Overflow
ISR_NOERR 5		# Bound Range Exceeded
ISR_NOERR 6		# Invalid Opcode
ISR_NOERR 7		# Device Not Available
ISR_ERR   8		# Double Fault
ISR_NOERR 9		# Coprocessor Segment Overrun
ISR_ERR   10		# Invalid TSS
ISR_ERR   11		# Segment Not Present
ISR_ERR   12		# Stack-Segment Fault
ISR_ERR   13		# General Protection Fault
ISR_ERR   14		# Page Fault
ISR_NOERR 15		# Reserved
ISR_NOERR 16		# x87 Floating-Point Exception
ISR_NOERR 17		# Alignment Check
ISR_NOERR 18		# Machine Check
ISR_NOERR 19		# SIMD Floating-Point Exception
ISR_NOERR 20		# Virtualization Exception
ISR_ERR	  21		# Control Protection Exception
ISR_NOERR 22		# Reserved
ISR_NOERR 23		# Reserved
ISR_NOERR 24		# Reserved
ISR_NOERR 25		# Reserved
ISR_NOERR 26		# Reserved
ISR_NOERR 27		# Reserved
ISR_NOERR 28		# Hypervisor Injection Exception
ISR_ERR   29		# VMM Communication Exception
ISR_ERR   30		# Security Exception
ISR_NOERR 31		# Reserved

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

interrupt_common_stub:
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	push %esp
	call interrupt_handler
	add $4, %esp

	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa

	add $8, %esp
	iret

.global isr_stub_table
isr_stub_table:
	.long isr_stub_0, isr_stub_1, isr_stub_2, isr_stub_3
	.long isr_stub_4, isr_stub_5, isr_stub_6, isr_stub_7
	.long isr_stub_8, isr_stub_9, isr_stub_10, isr_stub_11
	.long isr_stub_12, isr_stub_13, isr_stub_14, isr_stub_15
	.long isr_stub_16, isr_stub_17, isr_stub_18, isr_stub_19
	.long isr_stub_20, isr_stub_21, isr_stub_22, isr_stub_23
	.long isr_stub_24, isr_stub_25, isr_stub_26, isr_stub_27
	.long isr_stub_28, isr_stub_29, isr_stub_30, isr_stub_31

.global irq_stub_table
irq_stub_table:
	.long irq_stub_0, irq_stub_1, irq_stub_2, irq_stub_3
	.long irq_stub_4, irq_stub_5, irq_stub_6, irq_stub_7
	.long irq_stub_8, irq_stub_9, irq_stub_10, irq_stub_11
	.long irq_stub_12, irq_stub_13, irq_stub_14, irq_stub_15
