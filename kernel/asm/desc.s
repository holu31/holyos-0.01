.global gdt_flush
gdt_flush:
	movl 4(%esp), %eax
	lgdt (%eax)
	ljmp $0x08, $.flush
.flush:
	movw $0x10, %ax 
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	ret

.global idt_load
idt_load:
	movl 4(%esp), %eax
	lidt (%eax)
	ret
